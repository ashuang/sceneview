// Copyright [2015] Albert Huang

#include "sceneview/stock_resources.hpp"

#include <cassert>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

#include <QVector4D>

namespace sv {

const QString kColor = "color";
const QString kDiffuse = "diffuse";
const QString kSpecular = "specular";
const QString kShininess = "shininess";
const QString kTexture0 = "texture0";

struct StockResources::Priv {
  ResourceManager::Ptr resources;
};

StockResources::StockResources(const ResourceManager::Ptr& resources)
    : p_(new Priv) {
  p_->resources = resources;
}

StockResources::~StockResources() { delete p_; }

static GeometryResource::Ptr GetOrMakeGeometry(
    const QString& name, std::function<GeometryData()> data_function,
    const ResourceManager::Ptr& res) {
  GeometryResource::Ptr geom = res->GetGeometry(name);
  if (!geom) {
    geom = res->MakeGeometry(name);
    geom->Load(data_function());
  }
  return geom;
}

GeometryResource::Ptr StockResources::Cone() {
  return GetOrMakeGeometry("geom:sv_cone", ConeData, p_->resources);
}

GeometryResource::Ptr StockResources::Cube() {
  return GetOrMakeGeometry("geom:sv_cube", CubeData, p_->resources);
}

GeometryResource::Ptr StockResources::Cylinder() {
  return GetOrMakeGeometry("geom:sv_cylinder", CylinderData, p_->resources);
}

GeometryResource::Ptr StockResources::Sphere() {
  return GetOrMakeGeometry("geom:sv_sphere", SphereData, p_->resources);
}

Drawable::Ptr StockResources::UnitAxes() {
  GeometryResource::Ptr geom =
      GetOrMakeGeometry("geom:sv_unit_axes", UnitAxesData, p_->resources);

  MaterialResource::Ptr material =
      p_->resources->GetMaterial("mat:sv_unit_axes");
  if (!material) {
    material = p_->resources->MakeMaterial(Shader(kPerVertexColorLighting));
  }
  return Drawable::Create(geom, material);
}

struct StockShaderData {
  StockResources::StockShaderId id;
  QString fname_stem;
  QString preamble;
};

static std::vector<StockShaderData> g_stock_shader_data = {
    {StockResources::kUniformColorNoLighting, "no_lighting",
     "#define COLOR_UNIFORM\n"},
    {StockResources::kPerVertexColorNoLighting, "no_lighting",
     "#define COLOR_PER_VERTEX\n"},
    {StockResources::kTextureUniformColorNoLighting, "no_lighting",
     "#define COLOR_UNIFORM\n#define USE_TEXTURE0\n"},
    {StockResources::kUniformColorLighting, "lighting",
     "#define COLOR_UNIFORM\n"},
    {StockResources::kPerVertexColorLighting, "lighting",
     "#define COLOR_PER_VERTEX\n"},
    {StockResources::kTextureUniformColorLighting, "lighting",
     "#define COLOR_UNIFORM\n#define USE_TEXTURE0\n"},
    {StockResources::kBillboardTextured, "billboard", "#define USE_TEXTURE0\n"},
    {StockResources::kBillboardUniformColor, "billboard",
     "#define COLOR_UNIFORM\n"}};

static const StockShaderData& GetStockShaderData(
    StockResources::StockShaderId id) {
  for (auto& item : g_stock_shader_data) {
    if (item.id == id) {
      return item;
    }
  }
  throw std::invalid_argument("Invalid stock shader id");
}

ShaderResource::Ptr StockResources::Shader(StockShaderId id) {
  const StockShaderData& sdata = GetStockShaderData(id);
  const QString shader_name = "sv_stock_shader:" + QString::number(sdata.id);

  ShaderResource::Ptr shader = p_->resources->GetShader(shader_name);
  if (!shader) {
    shader = p_->resources->MakeShader(shader_name);
    shader->LoadFromFiles(":sceneview/stock_shaders/" + sdata.fname_stem,
                          sdata.preamble);
    if (!shader) {
      shader.reset();
    }
  }
  return shader;
}

MaterialResource::Ptr StockResources::NewMaterial(StockShaderId id) {
  return p_->resources->MakeMaterial(Shader(id));
}

GeometryData StockResources::CubeData() {
  GeometryData result;
  const double t = 0.5;

  // TODO(albert) use GL_TRIANGLE_STRIP instead of GL_TRIANGLES
  result.gl_mode = GL_TRIANGLES;

  result.vertices = {QVector3D(t, t, t),    QVector3D(t, -t, t),
                     QVector3D(t, -t, -t),  QVector3D(t, t, -t),

                     QVector3D(t, t, t),    QVector3D(t, t, -t),
                     QVector3D(-t, t, -t),  QVector3D(-t, t, t),

                     QVector3D(t, t, t),    QVector3D(-t, t, t),
                     QVector3D(-t, -t, t),  QVector3D(t, -t, t),

                     QVector3D(-t, t, t),   QVector3D(-t, t, -t),
                     QVector3D(-t, -t, -t), QVector3D(-t, -t, t),

                     QVector3D(t, -t, t),   QVector3D(-t, -t, t),
                     QVector3D(-t, -t, -t), QVector3D(t, -t, -t),

                     QVector3D(t, t, -t),   QVector3D(t, -t, -t),
                     QVector3D(-t, -t, -t), QVector3D(-t, t, -t)};

  for (int i = 0; i < 4; ++i) {
    result.normals.emplace_back(1, 0, 0);
  }
  for (int i = 0; i < 4; ++i) {
    result.normals.emplace_back(0, 1, 0);
  }
  for (int i = 0; i < 4; ++i) {
    result.normals.emplace_back(0, 0, 1);
  }
  for (int i = 0; i < 4; ++i) {
    result.normals.emplace_back(-1, 0, 0);
  }
  for (int i = 0; i < 4; ++i) {
    result.normals.emplace_back(0, -1, 0);
  }
  for (int i = 0; i < 4; ++i) {
    result.normals.emplace_back(0, 0, -1);
  }

  result.indices = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
                    8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
                    16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

  return result;
}

/**
 * Subdivides the triangles of a sphere.
 *
 * Algorithm:
 *   For each triangle in the original sphere:
 *     - Calculate the 3 midpoints of the original triangle edges.
 *     - Add each midopint vertex to new_vertices if it's not already there
 *     - Divide the triangle into 4 triangles, triforce-style, and add the
 *       vertex indices of each to new_indices.
 *     - Ensure the normals of the new triangle are in good shape.
 */
struct SphereSubdivider {
  typedef std::pair<int, int> VertexPair;

  explicit SphereSubdivider(const GeometryData& input) : input(input) {
    output.gl_mode = GL_TRIANGLES;
    output.vertices = input.vertices;

    for (size_t i = 0; i < input.indices.size(); i += 3) {
      const int index0 = input.indices[i];
      const int index1 = input.indices[i + 1];
      const int index2 = input.indices[i + 2];

      const int index01 = EdgeMidpoint(index0, index1);
      const int index12 = EdgeMidpoint(index1, index2);
      const int index02 = EdgeMidpoint(index0, index2);

      output.indices.push_back(index0);
      output.indices.push_back(index01);
      output.indices.push_back(index02);

      output.indices.push_back(index01);
      output.indices.push_back(index1);
      output.indices.push_back(index12);

      output.indices.push_back(index01);
      output.indices.push_back(index12);
      output.indices.push_back(index02);

      output.indices.push_back(index02);
      output.indices.push_back(index12);
      output.indices.push_back(index2);
    }
  }

  int EdgeMidpoint(int v0_index, int v1_index) {
    VertexPair edge;
    if (v0_index < v1_index) {
      edge = VertexPair(v0_index, v1_index);
    } else {
      edge = VertexPair(v1_index, v0_index);
    }
    auto iter = midpoint_indices.find(edge);
    if (iter != midpoint_indices.end()) {
      return iter->second;
    } else {
      const float radius = 0.5;
      const auto& v0 = input.vertices[v0_index];
      const auto& v1 = input.vertices[v1_index];
      const int midpoint_index = output.vertices.size();
      output.vertices.emplace_back(radius * ((v0 + v1) / 2).normalized());
      midpoint_indices[edge] = midpoint_index;
      return midpoint_index;
    }
  }

  const GeometryData& input;
  std::map<VertexPair, int> midpoint_indices;
  GeometryData output;
};

GeometryData StockResources::SphereData() {
  GeometryData result;
  result.gl_mode = GL_TRIANGLES;
  result.vertices = {QVector3D(0.000000, 0.000000, 0.500000),
                     QVector3D(0.364727, 0.264990, 0.216227),
                     QVector3D(-0.139313, 0.428763, 0.216227),
                     QVector3D(-0.450828, 0.000000, 0.216227),
                     QVector3D(-0.139313, -0.428763, 0.216227),
                     QVector3D(0.364727, -0.264990, 0.216227),
                     QVector3D(0.139313, 0.428763, -0.216227),
                     QVector3D(-0.364727, 0.264990, -0.216227),
                     QVector3D(-0.364727, -0.264990, -0.216227),
                     QVector3D(0.139313, -0.428763, -0.216227),
                     QVector3D(0.450828, -0.000000, -0.216227),
                     QVector3D(-0.000000, -0.000000, -0.500000)};
  result.indices = {0,  1,  2,  0,  2, 3, 0,  3, 4, 0,  4, 5, 0,  5,  1,
                    1,  10, 6,  2,  6, 7, 3,  7, 8, 4,  8, 9, 5,  9,  10,
                    1,  6,  2,  2,  7, 3, 3,  8, 4, 4,  9, 5, 5,  10, 1,
                    11, 6,  10, 11, 7, 6, 11, 8, 7, 11, 9, 8, 11, 10, 9};
  const int num_subdivisions = 2;
  for (int i = 0; i < num_subdivisions; ++i) {
    result = SphereSubdivider(result).output;
  }
  result.normals.resize(result.vertices.size());
  for (size_t i = 0; i < result.vertices.size(); ++i) {
    result.normals[i] = result.vertices[i] * 2;
  }
  return result;
}

GeometryData StockResources::ConeData() {
  GeometryData result;

  const double radius = 0.5;
  const double height = 1.0;
  const double half_height = height / 2;
  const int num_slices = 16;

  const double dtheta = 2 * M_PI / num_slices;
  const double half_dtheta = dtheta / 2;

  result.gl_mode = GL_TRIANGLES;

  std::vector<float> x_pts(num_slices);
  std::vector<float> y_pts(num_slices);
  for (int i = 0; i < num_slices; ++i) {
    const double theta = i * dtheta;
    x_pts[i] = radius * cos(theta);
    y_pts[i] = radius * sin(theta);
  }

  // Sides
  const double phi = atan2(radius, height);
  const double cosphi = cos(phi);
  const double sinphi = sin(phi);
  for (int i = 0; i < num_slices; ++i) {
    result.vertices.emplace_back(0, 0, half_height);
    result.vertices.emplace_back(x_pts[i], y_pts[i], -half_height);
    result.normals.emplace_back(cos(i * dtheta + half_dtheta) * cosphi,
                                sin(i * dtheta + half_dtheta) * cosphi, sinphi);
    result.normals.emplace_back(x_pts[i] * cosphi, y_pts[i] * cosphi, sinphi);
    result.indices.push_back(i * 2);
    result.indices.push_back(i * 2 + 1);
    result.indices.push_back(((i + 1) % num_slices) * 2 + 1);
  }

  // Bottom
  const int bottom_start_index = result.vertices.size();
  result.vertices.emplace_back(0, 0, -half_height);
  result.normals.emplace_back(0, 0, -1);
  for (int i = 0; i < num_slices; ++i) {
    result.vertices.emplace_back(x_pts[i], y_pts[i], -half_height);
    result.normals.emplace_back(0, 0, -1);
    result.indices.push_back(bottom_start_index + i + 1);
    result.indices.push_back(bottom_start_index);
    result.indices.push_back(bottom_start_index + (i + 1) % num_slices + 1);
  }

  return result;
}

GeometryData StockResources::CylinderData() {
  GeometryData result;
  result.gl_mode = GL_TRIANGLES;

  const double radius = 0.5;
  const double half_height = 0.5;
  const int num_slices = 16;

  std::vector<float> x_pts(num_slices);
  std::vector<float> y_pts(num_slices);
  for (int i = 0; i < num_slices; ++i) {
    const double theta = i * (2 * M_PI / num_slices);
    x_pts[i] = radius * cos(theta);
    y_pts[i] = radius * sin(theta);
  }

  // Top disc
  result.vertices.emplace_back(0, 0, half_height);
  result.normals.emplace_back(0, 0, 1);
  for (int i = 0; i < num_slices; ++i) {
    result.vertices.emplace_back(x_pts[i], y_pts[i], half_height);
    result.normals.emplace_back(0, 0, 1);
    result.indices.push_back(i + 1);
    result.indices.push_back((i + 1) % num_slices + 1);
    result.indices.push_back(0);
  }

  // Middle
  const int middle_start_index = result.vertices.size();
  for (int i = 0; i < num_slices; ++i) {
    const int i_next = (i + 1) % num_slices;
    result.vertices.emplace_back(x_pts[i], y_pts[i], half_height);
    result.vertices.emplace_back(x_pts[i], y_pts[i], -half_height);
    result.normals.emplace_back(2 * x_pts[i], 2 * y_pts[i], 0);
    result.normals.emplace_back(2 * x_pts[i], 2 * y_pts[i], 0);
    result.indices.push_back(middle_start_index + i * 2);
    result.indices.push_back(middle_start_index + i * 2 + 1);
    result.indices.push_back(middle_start_index + i_next * 2);

    result.indices.push_back(middle_start_index + i_next * 2);
    result.indices.push_back(middle_start_index + i * 2 + 1);
    result.indices.push_back(middle_start_index + i_next * 2 + 1);
  }

  // Bottom disc
  const int bottom_start_index = result.vertices.size();
  result.vertices.emplace_back(0, 0, -half_height);
  result.normals.emplace_back(0, 0, -1);
  for (int i = 0; i < num_slices; ++i) {
    result.vertices.emplace_back(x_pts[i], y_pts[i], -half_height);
    result.normals.emplace_back(0, 0, -1);
    result.indices.push_back(bottom_start_index + i + 1);
    result.indices.push_back(bottom_start_index);
    result.indices.push_back(bottom_start_index + (i + 1) % num_slices + 1);
  }

  return result;
}

GeometryData StockResources::UnitAxesData() {
  GeometryData cylinder = CylinderData();
  GeometryData result;
  result.gl_mode = cylinder.gl_mode;

  QMatrix4x4 scale_mat;
  scale_mat.scale(0.07, 0.07, 1);

  QMatrix4x4 x_rot_mat;
  x_rot_mat.rotate(90, 0, 1, 0);

  QMatrix4x4 x_trans_mat;
  x_trans_mat.translate(0.5, 0, 0);

  const QMatrix4x4 x_mat = x_trans_mat * x_rot_mat * scale_mat;
  for (size_t vert_ind = 0; vert_ind < cylinder.vertices.size(); ++vert_ind) {
    result.vertices.push_back(x_mat * cylinder.vertices[vert_ind]);
    result.normals.push_back(x_rot_mat * cylinder.normals[vert_ind]);
    result.diffuse.emplace_back(1.0, 0, 0, 1.0);
  }
  for (size_t index_ind = 0; index_ind < cylinder.indices.size(); ++index_ind) {
    result.indices.push_back(cylinder.indices[index_ind]);
  }

  QMatrix4x4 y_rot_mat;
  y_rot_mat.rotate(90, -1, 0, 0);

  QMatrix4x4 y_trans_mat;
  y_trans_mat.translate(0, 0.5, 0);

  const QMatrix4x4 y_mat = y_trans_mat * y_rot_mat * scale_mat;
  const int y_vertex_offset = result.vertices.size();
  for (size_t vert_ind = 0; vert_ind < cylinder.vertices.size(); ++vert_ind) {
    result.vertices.push_back(y_mat * cylinder.vertices[vert_ind]);
    result.normals.push_back(y_rot_mat * cylinder.normals[vert_ind]);
    result.diffuse.emplace_back(0.0, 1.0, 0, 1.0);
  }
  for (size_t index_ind = 0; index_ind < cylinder.indices.size(); ++index_ind) {
    result.indices.push_back(cylinder.indices[index_ind] + y_vertex_offset);
  }

  QMatrix4x4 z_trans_mat;
  z_trans_mat.translate(0, 0, 0.5);

  const int z_vertex_offset = result.vertices.size();
  const QMatrix4x4 z_mat = z_trans_mat * scale_mat;
  for (size_t vert_ind = 0; vert_ind < cylinder.vertices.size(); ++vert_ind) {
    result.vertices.push_back(z_mat * cylinder.vertices[vert_ind]);
    result.normals.push_back(cylinder.normals[vert_ind]);
    result.diffuse.emplace_back(0.0, 0.0, 1.0, 1.0);
  }
  for (size_t index_ind = 0; index_ind < cylinder.indices.size(); ++index_ind) {
    result.indices.push_back(cylinder.indices[index_ind] + z_vertex_offset);
  }

  return result;
}

}  // namespace sv
