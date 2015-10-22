// Copyright [2015] Albert Huang

#include "sceneview/internal_gl.hpp"

#include "sceneview/draw_context.hpp"

#include <cmath>
#include <vector>

#include <QOpenGLTexture>

#include "sceneview/camera_node.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/light_node.hpp"
#include "sceneview/mesh_node.hpp"
#include "sceneview/resource_manager.hpp"
#include "sceneview/scene_node.hpp"
#include "sceneview/stock_resources.hpp"

#if 0
#define dbg(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define dbg(...)
#endif

namespace sv {

DrawContext::DrawContext(const ResourceManager::Ptr& resources,
    const Scene::Ptr& scene) :
  resources_(resources),
  scene_(scene),
  cur_camera_(nullptr),
  bounding_box_mesh_(nullptr),
  draw_bounding_boxes_(false) {}

void DrawContext::Draw(CameraNode* camera) {
  cur_camera_ = camera;

  // TODO(albert) sort the meshes

  // render each mesh
  for (MeshNode* mesh : scene_->Meshes()) {
    if (mesh->Visible()) {
      DrawMesh(mesh);
    }

    if (draw_bounding_boxes_) {
      const AxisAlignedBox box_orig = mesh->GeometryBoundingBox();
      const AxisAlignedBox box = box_orig.Transformed(model_mat_);
      DrawBoundingBox(box);
    }
  }

  cur_camera_ = nullptr;
}

void DrawContext::DrawMesh(MeshNode* mesh) {
  // Compute the model matrix and check visibility
  model_mat_ = mesh->GetTransform();
  for (SceneNode* node = mesh->ParentNode(); node; node = node->ParentNode()) {
    model_mat_ = node->GetTransform() * model_mat_;
    if (!node->Visible()) {
      return;
    }
  }

  for (const Drawable::Ptr& drawable : mesh->Drawables()) {
    geometry_ = drawable->Geometry();
    material_ = drawable->Material();
    shader_ = material_->Shader();

    // Activate the shader program
    if (!shader_) {
      continue;
    }
    program_ = shader_->Program();
    if (!program_) {
      continue;
    }

    ActivateMaterial();

    DrawGeometry();

    GLenum gl_err = glGetError();
    if (gl_err != GL_NO_ERROR) {
      printf("OpenGL: %s\n", sv::glErrorString(gl_err));
    }

    // Done. Release resources
    program_->release();

    // If we called glPointSize() earlier, then reset the value.
    if (material_->PointSize() > 0) {
      glPointSize(1);
    }

    if (material_->LineWidth() > 0) {
      glLineWidth(1);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
}

void DrawContext::ActivateMaterial() {
  program_->bind();

  glFrontFace(GL_CCW);

  // set OpenGL attributes based on material properties.
  if (material_->TwoSided()) {
    glDisable(GL_CULL_FACE);
  } else {
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
  }

  if (material_->DepthTest()) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  if (material_->DepthWrite()) {
    glDepthMask(GL_TRUE);
  } else {
    glDepthMask(GL_FALSE);
  }

  if (material_->ColorWrite()) {
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  } else {
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  }

  const float point_size = material_->PointSize();
  if (point_size > 0) {
    glPointSize(point_size);
  }

  const float line_width = material_->LineWidth();
  if (line_width > 0) {
    glLineWidth(line_width);
  }

  if (material_->Blend()) {
    glEnable(GL_BLEND);
    GLenum sfactor;
    GLenum dfactor;
    material_->BlendFunc(&sfactor, &dfactor);
    glBlendFunc(sfactor, dfactor);
  } else {
    glDisable(GL_BLEND);
  }

  // Set shader standard variables
  const ShaderStandardVariables& locs = shader_->StandardVariables();
  const QMatrix4x4 proj_mat = cur_camera_->GetProjectionMatrix();
  const QMatrix4x4 view_mat = cur_camera_->GetViewMatrix();

  // Set uniform variables
  if (locs.sv_proj_mat >= 0) {
    program_->setUniformValue(locs.sv_proj_mat, proj_mat);
  }
  if (locs.sv_view_mat >= 0) {
    program_->setUniformValue(locs.sv_view_mat, view_mat);
  }
  if (locs.sv_view_mat_inv >= 0) {
    program_->setUniformValue(locs.sv_view_mat_inv, view_mat.inverted());
  }
  if (locs.sv_model_mat >= 0) {
    program_->setUniformValue(locs.sv_model_mat, model_mat_);
  }
  if (locs.sv_mvp_mat >= 0) {
    program_->setUniformValue(locs.sv_mvp_mat,
        proj_mat * view_mat * model_mat_);
  }
  if (locs.sv_mv_mat >= 0) {
    program_->setUniformValue(locs.sv_mv_mat, view_mat * model_mat_);
  }
  if (locs.sv_model_normal_mat >= 0) {
    program_->setUniformValue(locs.sv_model_normal_mat,
        model_mat_.normalMatrix());
  }

  const std::vector<LightNode*>& lights = scene_->Lights();
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
      program_->setUniformValue(light_loc.is_directional, is_directional);
    }

    if (light_loc.direction >= 0) {
      const QVector3D light_dir = light_node->Direction();
      program_->setUniformValue(light_loc.direction, light_dir);
    }

    if (light_loc.position >= 0) {
      const QVector3D light_pos = light_node->Translation();
      program_->setUniformValue(light_loc.position, light_pos);
    }

    if (light_loc.ambient >= 0) {
      const float ambient = light_node->Ambient();
      program_->setUniformValue(light_loc.ambient, ambient);
    }

    if (light_loc.color >= 0) {
      const QVector3D color = light_node->Color();
      program_->setUniformValue(light_loc.color, color);
    }

    if (light_loc.attenuation >= 0) {
      const float attenuation = light_node->Attenuation();
      program_->setUniformValue(light_loc.attenuation, attenuation);
    }

    if (light_loc.cone_angle >= 0) {
      const float cone_angle = light_node->ConeAngle() * M_PI / 180;
      program_->setUniformValue(light_loc.cone_angle, cone_angle);
    }
  }

  // Load shader uniform variables from the material
  for (auto& item : material_->ShaderParameters()) {
    ShaderUniform& uniform = item.second;
    uniform.LoadToProgram(program_);
  }

  // Load textures
  unsigned int texunit = 0;
  for (auto& item : *(material_->GetTextures())) {
    const QString& texname = item.first;
    QOpenGLTexture* texture = item.second;
    texture->bind(texunit);
    program_->setUniformValue(texname.toStdString().c_str(), texunit);
  }
}

static void SetupAttributeArray(QOpenGLShaderProgram* program,
    int location, int num_attributes,
    GLenum attr_type, int offset, int attribute_size) {
  if (location < 0) {
    return;
  }

  if (num_attributes > 0) {
        program->enableAttributeArray(location);
        program->setAttributeBuffer(location,
            attr_type, offset, attribute_size, 0);
  } else {
    program->disableAttributeArray(location);
  }
}

void DrawContext::DrawGeometry() {
  // Load the mesh geometry and bind its vertex buffer
  QOpenGLBuffer* vbo = geometry_->VBO();
  vbo->bind();

  if (program_) {
    const ShaderStandardVariables& locs = shader_->StandardVariables();

    // Per-vertex attribute arrays
    SetupAttributeArray(program_, locs.sv_vert_pos,
        geometry_->NumVertices(), GL_FLOAT, geometry_->VertexOffset(), 3);
    SetupAttributeArray(program_, locs.sv_normal,
        geometry_->NumNormals(), GL_FLOAT, geometry_->NormalOffset(), 3);
    SetupAttributeArray(program_, locs.sv_diffuse,
        geometry_->NumDiffuse(), GL_FLOAT, geometry_->DiffuseOffset(), 4);
    SetupAttributeArray(program_, locs.sv_specular,
        geometry_->NumSpecular(), GL_FLOAT, geometry_->SpecularOffset(), 4);
    SetupAttributeArray(program_, locs.sv_shininess,
        geometry_->NumShininess(), GL_FLOAT, geometry_->ShininessOffset(), 1);
    SetupAttributeArray(program_, locs.sv_tex_coords_0,
        geometry_->NumTexCoords0(), GL_FLOAT, geometry_->TexCoords0Offset(), 2);
  }

  // Draw the mesh geometry
  QOpenGLBuffer* index_buffer = geometry_->IndexBuffer();
  if (index_buffer) {
    index_buffer->bind();
    glDrawElements(geometry_->GLMode(), geometry_->NumIndices(),
        geometry_->IndexType(), 0);
    index_buffer->release();
  } else {
    glDrawArrays(geometry_->GLMode(), 0, geometry_->NumVertices());
  }
  vbo->release();
}

void DrawContext::DrawBoundingBox(const AxisAlignedBox& box) {
  if (!bounding_box_mesh_) {
    StockResources stock(resources_);
    ShaderResource::Ptr shader =
      stock.Shader(StockResources::kUniformColorNoLighting);

    MaterialResource::Ptr material = resources_->MakeMaterial(shader);
    material->SetParam("color", 0.0f, 1.0f, 0.0f, 1.0f);

    GeometryResource::Ptr geometry = resources_->MakeGeometry();
    GeometryData gdata;
    gdata.gl_mode = GL_LINES;
    gdata.vertices = {
      { QVector3D(0, 0, 0) },
      { QVector3D(0, 1, 0) },
      { QVector3D(1, 1, 0) },
      { QVector3D(1, 0, 0) },
      { QVector3D(0, 0, 1) },
      { QVector3D(0, 1, 1) },
      { QVector3D(1, 1, 1) },
      { QVector3D(1, 0, 1) },
    };
    gdata.indices = {
      0, 1, 1, 2, 2, 3, 3, 0,
      4, 5, 5, 6, 6, 7, 7, 4,
      0, 4, 1, 5, 2, 6, 3, 7 };
    geometry->Load(gdata);

    bounding_box_mesh_ = scene_->MakeMesh(nullptr);
    bounding_box_mesh_->Add(geometry, material);

    // hack to prevent the bounding box to appear during normal rendering
    bounding_box_mesh_->SetVisible(false);
  }

  bounding_box_mesh_->SetScale(box.Max() - box.Min());
  bounding_box_mesh_->SetTranslation(box.Min());

  DrawMesh(bounding_box_mesh_);
}

}  // namespace sv
