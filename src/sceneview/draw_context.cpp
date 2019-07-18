// Copyright [2015] Albert Huang

#include "sceneview/internal_gl.hpp"

#include "sceneview/draw_context.hpp"

#include <cmath>
#include <vector>

#include <QOpenGLTexture>

#include "sceneview/camera_node.hpp"
#include "sceneview/drawable.hpp"
#include "sceneview/draw_group.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/light_node.hpp"
#include "sceneview/plane.hpp"
#include "sceneview/renderer.hpp"
#include "sceneview/resource_manager.hpp"
#include "sceneview/scene_node.hpp"
#include "sceneview/stock_resources.hpp"

#if 0
#define dbg(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define dbg(...)
#endif

using std::max;

namespace sv {

struct DrawNodeData {
  DrawNode* node = nullptr;
  float squared_distance = 0;
  QMatrix4x4 model_mat;
  AxisAlignedBox world_bbox;
};

/**
 * Frustum data structure used for view frustum culling.
 */
class Frustum {
 public:
  Frustum(CameraNode* camera);

  bool Intersects(const AxisAlignedBox& box);

 private:
  std::vector<Plane> planes_;
};

Frustum::Frustum(CameraNode* camera) {
  // Compute the frustum planes
  QSize viewport_size = camera->GetViewportSize();
  const int x1 = viewport_size.width();
  const int y1 = viewport_size.height();

  QVector3D top_left_start;
  QVector3D bot_left_start;
  QVector3D top_right_start;
  QVector3D bot_right_start;

  QVector3D top_left_end = camera->Unproject(0, 0, 1);
  QVector3D bot_left_end = camera->Unproject(0, y1, 1);
  QVector3D top_right_end = camera->Unproject(x1, 0, 1);
  QVector3D bot_right_end = camera->Unproject(x1, y1, 1);

  if (camera->GetProjectionType() == sv::CameraNode::kOrthographic) {
    top_left_start = camera->Unproject(0, 0, 0);
    bot_left_start = camera->Unproject(0, y1, 0);
    top_right_start = camera->Unproject(x1, 0, 0);
    bot_right_start = camera->Unproject(x1, y1, 0);
  } else {
    const QVector3D eye = camera->WorldTransform().map(QVector3D(0, 0, 0));
    top_left_start = eye;
    bot_left_start = eye;
    top_right_start = eye;
    bot_right_start = eye;
  }

  const QVector3D top_left_dir = (top_left_end - top_left_start).normalized();
  const QVector3D bot_left_dir = (bot_left_end - bot_left_start).normalized();
  const QVector3D top_right_dir = (top_right_end - top_right_start).normalized();
  const QVector3D bot_right_dir = (bot_right_end - bot_right_start).normalized();

  const double near = camera->GetZNear();
  const double far = camera->GetZFar();

  const QVector3D ntl = top_left_start + near * top_left_dir;
  const QVector3D ntr = top_right_start + near * top_right_dir;
  const QVector3D nbl = bot_left_start + near * bot_left_dir;
  const QVector3D nbr = bot_right_start + near * bot_right_dir;
  const QVector3D ftl = top_left_start + far * top_left_dir;
  const QVector3D ftr = top_right_start + far * top_right_dir;
  const QVector3D fbl = bot_left_start + far * bot_left_dir;
  const QVector3D fbr = bot_right_start + far * bot_right_dir;

  planes_.push_back(Plane::FromThreePoints(ntr, ftl, ftr));  // top
  planes_.push_back(Plane::FromThreePoints(nbr, fbr, fbl));  // bottom
  planes_.push_back(Plane::FromThreePoints(ntl, nbl, fbl));  // left
  planes_.push_back(Plane::FromThreePoints(ntr, fbr, nbr));  // right
  planes_.push_back(Plane::FromThreePoints(ntl, ntr, nbr));  // near
  planes_.push_back(Plane::FromThreePoints(ftl, fbr, ftr));  // far
}

bool Frustum::Intersects(const AxisAlignedBox& box) {
  const QVector3D& bmin = box.Min();
  const QVector3D& bmax = box.Max();
  for (const Plane& plane : planes_) {
    const QVector3D& normal = plane.Normal();
    const QVector3D test_point(normal.x() > 0 ? bmax.x() : bmin.x(),
                               normal.y() > 0 ? bmax.y() : bmin.y(),
                               normal.z() > 0 ? bmax.z() : bmin.z());
    if (plane.SignedDistance(test_point) < 0) {
      return false;
    }
  }
  return true;
}

static void CheckGLErrors(const QString& name) {
  GLenum err_code = glGetError();
  const char* err_str;
  while (err_code != GL_NO_ERROR) {
    err_str = sv::glErrorString(err_code);
    fprintf(stderr, "OpenGL Error (%s)\n", name.toStdString().c_str());
    fprintf(stderr, "%s\n", err_str);
    err_code = glGetError();
  }
}

static double squaredDistanceToAABB(const QVector3D& point,
                                    const AxisAlignedBox& box) {
  const QVector3D center = (box.Max() + box.Min()) / 2;
  const QVector3D half_sz = (box.Max() - box.Min()) / 2;
  const QVector3D vec(max(0.0, fabs(center.x() - point.x()) - half_sz.x()),
                      max(0.0, fabs(center.y() - point.y()) - half_sz.y()),
                      max(0.0, fabs(center.z() - point.z()) - half_sz.z()));
  return vec.lengthSquared();
}

struct DrawContext::Priv {
  ResourceManager::Ptr resources;

  Scene::Ptr scene;

  QColor clear_color;

  // Rendering variables
  int viewport_width = 0;
  int viewport_height = 0;
  CameraNode* cur_camera = nullptr;

  MaterialResource::Ptr material;
  GeometryResource::Ptr geometry;
  ShaderResource::Ptr shader;
  QOpenGLShaderProgram* program;
  QMatrix4x4 model_mat;

  std::vector<DrawGroup*> draw_groups;

  bool gl_two_sided;
  bool gl_depth_test;
  GLenum gl_depth_func;
  bool gl_depth_write;
  bool gl_color_write;
  float gl_point_size;
  float gl_line_width;
  bool gl_blend;
  GLenum gl_sfactor;
  GLenum gl_dfactor;

  // For debugging
  DrawNode* bounding_box_node;
  bool draw_bounding_boxes;
};

DrawContext::DrawContext(const ResourceManager::Ptr& resources,
                         const Scene::Ptr& scene) : p_(new Priv) {
  p_->resources = resources;
  p_->scene = scene;
  p_->clear_color = QColor(0, 0, 0, 255);
  p_->bounding_box_node = nullptr;
  p_->draw_bounding_boxes = false;
}

DrawContext::~DrawContext()
{
  delete p_;
}

void DrawContext::Draw(int viewport_width, int viewport_height,
                       std::vector<Renderer*>* prenderers) {
  p_->viewport_width = viewport_width;
  p_->viewport_height = viewport_height;
  p_->cur_camera = p_->scene->GetDefaultDrawGroup()->GetCamera();

  // Clear the drawing area
  glClearColor(p_->clear_color.redF(), p_->clear_color.greenF(), p_->clear_color.blueF(),
               p_->clear_color.alphaF());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::vector<Renderer*>& renderers = *prenderers;

  // Setup the fixed-function pipeline.
  PrepareFixedFunctionPipeline();

  // Inform the renderers that drawing is about to begin
  for (Renderer* renderer : renderers) {
    if (renderer->Enabled()) {
      glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_POLYGON_STIPPLE_BIT |
                   GL_POLYGON_BIT | GL_LINE_BIT | GL_FOG_BIT | GL_LIGHTING_BIT);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      renderer->RenderBegin();
      CheckGLErrors(renderer->Name());
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPopAttrib();
    }
  }

  // Set some OpenGL state to a known configuration
  p_->gl_two_sided = false;
  glDisable(GL_CULL_FACE);
  p_->gl_depth_test = true;
  glEnable(GL_DEPTH_TEST);
  p_->gl_depth_func = GL_LESS;
  glDepthFunc(p_->gl_depth_func);
  p_->gl_depth_write = true;
  glDepthMask(GL_TRUE);
  p_->gl_color_write = true;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  p_->gl_point_size = 1;
  glPointSize(p_->gl_point_size);
  p_->gl_line_width = 1;
  glLineWidth(p_->gl_line_width);
  p_->gl_blend = false;
  glDisable(GL_BLEND);
  p_->gl_sfactor = GL_ONE;
  p_->gl_dfactor = GL_ZERO;
  glBlendFunc(p_->gl_sfactor, p_->gl_dfactor);

  // Draw nodes, ordered first by draw group.
  for (DrawGroup* dgroup : p_->draw_groups) {
    DrawDrawGroup(dgroup);
  }

  // Setup the fixed-function pipeline again.
  PrepareFixedFunctionPipeline();

  // Notify renderers that drawing has finished
  for (Renderer* renderer : renderers) {
    if (renderer->Enabled()) {
      glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_POLYGON_STIPPLE_BIT |
                   GL_POLYGON_BIT | GL_LINE_BIT | GL_FOG_BIT | GL_LIGHTING_BIT);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      renderer->RenderEnd();
      CheckGLErrors(renderer->Name());
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPopAttrib();
    }
  }

  p_->cur_camera = nullptr;
}

void DrawContext::SetClearColor(const QColor& color) { p_->clear_color = color; }

void DrawContext::SetDrawGroups(const std::vector<DrawGroup*>& groups) {
  p_->draw_groups = groups;
  std::sort(p_->draw_groups.begin(), p_->draw_groups.end(),
            [](const DrawGroup* draw_group_a, const DrawGroup* draw_group_b) {
              return draw_group_a->Order() < draw_group_b->Order();
            });
}

void DrawContext::PrepareFixedFunctionPipeline() {
  p_->cur_camera = p_->scene->GetDefaultDrawGroup()->GetCamera();

  // Enable the fixed function pipeline by disabling any active shader program.
  glUseProgram(0);

  // Setup the projection and view matrices
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMultMatrixf(p_->cur_camera->GetProjectionMatrix().constData());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(p_->cur_camera->GetViewMatrix().constData());

  // Setup lights
  const GLenum gl_lights[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
                              GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
  std::vector<LightNode*> lights = p_->scene->Lights();
  for (int light_ind = 0; light_ind < 8; ++light_ind) {
    const GLenum gl_light = gl_lights[light_ind];
    LightNode* light = lights[light_ind];
    const LightType light_type = light->GetLightType();

    if (light_type == LightType::kDirectional) {
      const QVector3D dir = light->Direction();
      const float dir4f[4] = {dir.x(), dir.y(), dir.z(), 0};
      glLightfv(gl_light, GL_POSITION, dir4f);
    } else {
      const QVector3D posf = light->Translation();
      const float pos4f[4] = {posf.x(), posf.y(), posf.z(), 1};
      glLightfv(gl_light, GL_POSITION, pos4f);

      const float attenuation = light->Attenuation();
      glLightf(gl_light, GL_QUADRATIC_ATTENUATION, attenuation);
      glLightf(gl_light, GL_CONSTANT_ATTENUATION, 1.0);

      if (light_type == LightType::kSpot) {
        const float cone_angle_deg = light->ConeAngle();
        glLightf(gl_light, GL_SPOT_CUTOFF, cone_angle_deg);
        glLightf(gl_light, GL_SPOT_EXPONENT, 1.2);
      }
    }

    const QVector3D& color = light->Color();
    const QVector3D& ambient = color * light->Ambient();
    const QVector3D& specular = color * light->Specular();
    const float color4f[4] = {color.x(), color.y(), color.z(), 1};
    const float ambient4f[4] = {ambient.x(), ambient.y(), ambient.z(), 1};
    const float specular4f[4] = {specular.x(), specular.y(), specular.z(), 1};
    glLightfv(gl_light, GL_AMBIENT, ambient4f);
    glLightfv(gl_light, GL_DIFFUSE, color4f);
    glLightfv(gl_light, GL_SPECULAR, specular4f);

    glEnable(gl_light);
    break;
  }

  // Set some default rendering parameters
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void DrawContext::DrawDrawGroup(DrawGroup* dgroup) {
  p_->cur_camera = dgroup->GetCamera();
  p_->cur_camera->SetViewportSize(p_->viewport_width, p_->viewport_height);

  Frustum frustum = p_->cur_camera;
  const QVector3D eye = p_->cur_camera->WorldTransform().map(QVector3D(0, 0, 0));

  // Figure out which nodes to draw and some data about them.
  std::vector<DrawNodeData> to_draw;
  const int num_draw_nodes = dgroup->DrawNodes().size();
  to_draw.reserve(num_draw_nodes);
  const bool do_frustum_culling = dgroup->GetFrustumCulling();

  for (DrawNode* draw_node : dgroup->DrawNodes()) {
    // If the node is not visible, then skip it.
    bool visible = true;
    for (SceneNode* node = draw_node; node; node = node->ParentNode()) {
      if (!node->Visible()) {
        visible = false;
        break;
      }
    }
    if (!visible) {
      continue;
    }

    // For each draw node, compute:
    //   - model matrix
    //   - world frame axis aligned bounding box
    //   - squared distance to camera
    //   - view frustum intersection
    DrawNodeData dndata;
    dndata.node = draw_node;

    // Cache the model mat matrix and world frame bounding box
    dndata.model_mat = draw_node->WorldTransform();

    // Compute the world frame axis-aligned bounding box
    dndata.world_bbox = draw_node->WorldBoundingBox();

    // View frustum culling
    if (do_frustum_culling && dndata.world_bbox.Valid() &&
        !frustum.Intersects(dndata.world_bbox)) {
      continue;
    }

    dndata.squared_distance = squaredDistanceToAABB(eye, dndata.world_bbox);

    to_draw.push_back(dndata);
  }

  switch (dgroup->GetNodeOrdering()) {
    case NodeOrdering::kBackToFront:
      // Sort nodes to draw back to front
      std::sort(to_draw.begin(), to_draw.end(),
                [](const DrawNodeData& dndata_a, const DrawNodeData& dndata_b) {
                  return dndata_a.node->DrawOrder() < dndata_b.node->DrawOrder() ||
                  dndata_a.squared_distance > dndata_b.squared_distance;
                });
      break;
    case NodeOrdering::kFrontToBack:
      // Sort nodes to draw front to back
      std::sort(to_draw.begin(), to_draw.end(),
                [](const DrawNodeData& dndata_a, const DrawNodeData& dndata_b) {
                  return dndata_a.node->DrawOrder() < dndata_b.node->DrawOrder() ||
                  dndata_a.squared_distance < dndata_b.squared_distance;
                });
      break;
    case NodeOrdering::kNone:
    default:
      // Sort nodes only by DrawOrder() vaules
      std::sort(to_draw.begin(), to_draw.end(),
                [](const DrawNodeData& dndata_a, const DrawNodeData& dndata_b) {
                  return dndata_a.node->DrawOrder() < dndata_b.node->DrawOrder();
                });
      break;
  }

  // Draw each draw node
  for (DrawNodeData& dndata : to_draw) {
    const QString name = dndata.node->Name();
    p_->model_mat = dndata.model_mat;
    DrawDrawNode(dndata.node);

    if (p_->draw_bounding_boxes) {
      DrawBoundingBox(dndata.world_bbox);
    }
  }
}

void DrawContext::DrawDrawNode(DrawNode* draw_node) {
  for (const Drawable::Ptr& drawable : draw_node->Drawables()) {
    p_->geometry = drawable->Geometry();
    p_->material = drawable->Material();
    p_->shader = p_->material->Shader();

    // Activate the shader program
    if (!p_->shader) {
      continue;
    }
    p_->program = p_->shader->Program();
    if (!p_->program) {
      continue;
    }

    ActivateMaterial();

    if (drawable->PreDraw()) {
      DrawGeometry();
    }

    drawable->PostDraw();

    GLenum gl_err = glGetError();
    if (gl_err != GL_NO_ERROR) {
      printf("OpenGL: %s\n", sv::glErrorString(gl_err));
    }

    // Done. Release resources
    p_->program->release();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
}

void DrawContext::ActivateMaterial() {
  p_->program->bind();

  glFrontFace(GL_CCW);

  // set OpenGL attributes based on material properties.
  const bool mat_two_sided = p_->material->TwoSided();
  if (mat_two_sided != p_->gl_two_sided) {
    p_->gl_two_sided = mat_two_sided;
    if (p_->gl_two_sided) {
      glDisable(GL_CULL_FACE);
    } else {
      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
    }
  }

  const bool mat_depth_test = p_->material->DepthTest();
  if (mat_depth_test != p_->gl_depth_test) {
    p_->gl_depth_test = mat_depth_test;
    if (p_->gl_depth_test) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  const GLenum mat_depth_func = p_->material->DepthFunc();
  if (mat_depth_func != p_->gl_depth_func) {
    p_->gl_depth_func = mat_depth_func;
    glDepthFunc(p_->gl_depth_func);
  }

  const bool mat_depth_write = p_->material->DepthWrite();
  if (p_->gl_depth_write != mat_depth_write) {
    p_->gl_depth_write = mat_depth_write;
    if (p_->gl_depth_write) {
      glDepthMask(GL_TRUE);
    } else {
      glDepthMask(GL_FALSE);
    }
  }

  const bool mat_color_write = p_->material->ColorWrite();
  if (p_->gl_color_write != mat_color_write) {
    p_->gl_color_write = mat_color_write;
    if (p_->gl_color_write) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    } else {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
  }

  const float mat_point_size = p_->material->PointSize();
  if (p_->gl_point_size != mat_point_size) {
    p_->gl_point_size = mat_point_size;
    glPointSize(p_->gl_point_size);
  }

  const float mat_line_width = p_->material->LineWidth();
  if (p_->gl_line_width != mat_line_width) {
    p_->gl_line_width = mat_line_width;
    glLineWidth(p_->gl_line_width);
  }

  const bool mat_blend = p_->material->Blend();
  if (p_->gl_blend != mat_blend) {
    p_->gl_blend = mat_blend;
    if (p_->gl_blend) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }

  GLenum mat_sfactor;
  GLenum mat_dfactor;
  p_->material->BlendFunc(&mat_sfactor, &mat_dfactor);
  if (p_->gl_sfactor != mat_sfactor || p_->gl_dfactor != mat_dfactor) {
    p_->gl_sfactor = mat_sfactor;
    p_->gl_dfactor = mat_dfactor;
    glBlendFunc(p_->gl_sfactor, p_->gl_dfactor);
  }

  // Set shader standard variables
  const ShaderStandardVariables& locs = p_->shader->StandardVariables();
  const QMatrix4x4 proj_mat = p_->cur_camera->GetProjectionMatrix();
  const QMatrix4x4 view_mat = p_->cur_camera->GetViewMatrix();

  // Set uniform variables
  if (locs.sv_proj_mat >= 0) {
    p_->program->setUniformValue(locs.sv_proj_mat, proj_mat);
  }
  if (locs.sv_view_mat >= 0) {
    p_->program->setUniformValue(locs.sv_view_mat, view_mat);
  }
  if (locs.sv_view_mat_inv >= 0) {
    p_->program->setUniformValue(locs.sv_view_mat_inv, view_mat.inverted());
  }
  if (locs.sv_model_mat >= 0) {
    p_->program->setUniformValue(locs.sv_model_mat, p_->model_mat);
  }
  if (locs.sv_mvp_mat >= 0) {
    p_->program->setUniformValue(locs.sv_mvp_mat,
                              proj_mat * view_mat * p_->model_mat);
  }
  if (locs.sv_mv_mat >= 0) {
    p_->program->setUniformValue(locs.sv_mv_mat, view_mat * p_->model_mat);
  }
  if (locs.sv_model_normal_mat >= 0) {
    p_->program->setUniformValue(locs.sv_model_normal_mat,
                              p_->model_mat.normalMatrix());
  }

  const std::vector<LightNode*>& lights = p_->scene->Lights();
  int num_lights = lights.size();
  if (num_lights > kShaderMaxLights) {
    printf("Too many lights. Max: %d\n", kShaderMaxLights);
    num_lights = kShaderMaxLights;
  }

  for (int light_ind = 0; light_ind < num_lights; ++light_ind) {
    const LightNode* light_node = lights[light_ind];
    const ShaderLightLocation light_loc = locs.sv_lights[light_ind];
    LightType light_type = light_node->GetLightType();

    if (light_loc.is_directional >= 0) {
      const bool is_directional = light_type == LightType::kDirectional;
      p_->program->setUniformValue(light_loc.is_directional, is_directional);
    }

    if (light_loc.direction >= 0) {
      const QVector3D light_dir = light_node->Direction();
      p_->program->setUniformValue(light_loc.direction, light_dir);
    }

    if (light_loc.position >= 0) {
      const QVector3D light_pos = light_node->Translation();
      p_->program->setUniformValue(light_loc.position, light_pos);
    }

    if (light_loc.ambient >= 0) {
      const float ambient = light_node->Ambient();
      p_->program->setUniformValue(light_loc.ambient, ambient);
    }

    if (light_loc.specular >= 0) {
      const float specular = light_node->Specular();
      p_->program->setUniformValue(light_loc.specular, specular);
    }

    if (light_loc.color >= 0) {
      const QVector3D color = light_node->Color();
      p_->program->setUniformValue(light_loc.color, color);
    }

    if (light_loc.attenuation >= 0) {
      const float attenuation = light_node->Attenuation();
      p_->program->setUniformValue(light_loc.attenuation, attenuation);
    }

    if (light_loc.cone_angle >= 0) {
      const float cone_angle = light_node->ConeAngle() * M_PI / 180;
      p_->program->setUniformValue(light_loc.cone_angle, cone_angle);
    }
  }

  // Load shader uniform variables from the material
  for (auto& item : p_->material->ShaderParameters()) {
    ShaderUniform& uniform = item.second;
    uniform.LoadToProgram(p_->program);
  }

  // Load textures
  unsigned int texunit = 0;
  for (auto& item : p_->material->GetTextures()) {
    const QString& texname = item.first;
    const std::shared_ptr<QOpenGLTexture>& texture = item.second;
    texture->bind(texunit);
    p_->program->setUniformValue(texname.toStdString().c_str(), texunit);
  }
}

static void SetupAttributeArray(QOpenGLShaderProgram* program, int location,
                                int num_attributes, GLenum attr_type,
                                int offset, int attribute_size) {
  if (location < 0) {
    return;
  }

  if (num_attributes > 0) {
    program->enableAttributeArray(location);
    program->setAttributeBuffer(location, attr_type, offset, attribute_size, 0);
  } else {
    program->disableAttributeArray(location);
  }
}

void DrawContext::DrawGeometry() {
  // Load geometry and bind a vertex buffer
  QOpenGLBuffer* vbo = p_->geometry->VBO();
  vbo->bind();

  // Load per-vertex attribute arrays
  const ShaderStandardVariables& locs = p_->shader->StandardVariables();
  SetupAttributeArray(p_->program, locs.sv_vert_pos, p_->geometry->NumVertices(),
                      GL_FLOAT, p_->geometry->VertexOffset(), 3);
  SetupAttributeArray(p_->program, locs.sv_normal, p_->geometry->NumNormals(),
                      GL_FLOAT, p_->geometry->NormalOffset(), 3);
  SetupAttributeArray(p_->program, locs.sv_diffuse, p_->geometry->NumDiffuse(),
                      GL_FLOAT, p_->geometry->DiffuseOffset(), 4);
  SetupAttributeArray(p_->program, locs.sv_specular, p_->geometry->NumSpecular(),
                      GL_FLOAT, p_->geometry->SpecularOffset(), 4);
  SetupAttributeArray(p_->program, locs.sv_shininess, p_->geometry->NumShininess(),
                      GL_FLOAT, p_->geometry->ShininessOffset(), 1);
  SetupAttributeArray(p_->program, locs.sv_tex_coords_0,
                      p_->geometry->NumTexCoords0(), GL_FLOAT,
                      p_->geometry->TexCoords0Offset(), 2);

  // TODO load custom attribute arrays

  // Draw the geometry
  QOpenGLBuffer* index_buffer = p_->geometry->IndexBuffer();
  if (index_buffer) {
    index_buffer->bind();
    glDrawElements(p_->geometry->GLMode(), p_->geometry->NumIndices(),
                   p_->geometry->IndexType(), 0);
    index_buffer->release();
  } else {
    glDrawArrays(p_->geometry->GLMode(), 0, p_->geometry->NumVertices());
  }
  vbo->release();
}

void DrawContext::DrawBoundingBox(const AxisAlignedBox& box) {
  if (!p_->bounding_box_node) {
    StockResources stock(p_->resources);
    ShaderResource::Ptr shader =
        stock.Shader(StockResources::kUniformColorNoLighting);

    MaterialResource::Ptr material = p_->resources->MakeMaterial(shader);
    material->SetParam("color", 0.0f, 1.0f, 0.0f, 1.0f);

    GeometryResource::Ptr geometry = p_->resources->MakeGeometry();
    GeometryData gdata;
    gdata.gl_mode = GL_LINES;
    gdata.vertices = {
        {QVector3D(0, 0, 0)}, {QVector3D(0, 1, 0)}, {QVector3D(1, 1, 0)},
        {QVector3D(1, 0, 0)}, {QVector3D(0, 0, 1)}, {QVector3D(0, 1, 1)},
        {QVector3D(1, 1, 1)}, {QVector3D(1, 0, 1)},
    };
    gdata.indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                     6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
    geometry->Load(gdata);

    p_->bounding_box_node = p_->scene->MakeDrawNode(nullptr);
    p_->bounding_box_node->Add(geometry, material);

    // hack to prevent the bounding box to appear during normal rendering
    p_->bounding_box_node->SetVisible(false);
  }

  p_->bounding_box_node->SetScale(box.Max() - box.Min());
  p_->bounding_box_node->SetTranslation(box.Min());
  p_->model_mat = p_->bounding_box_node->WorldTransform();

  DrawDrawNode(p_->bounding_box_node);
}

}  // namespace sv
