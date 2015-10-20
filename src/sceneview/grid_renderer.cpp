// Copyright [2015] Albert Huang

#include "sceneview/grid_renderer.hpp"

#include "sceneview/camera_node.hpp"
#include "sceneview/mesh_node.hpp"
#include "sceneview/stock_resources.hpp"
#include "sceneview/viewport.hpp"

namespace sceneview {

static double RoundTo125(double input) {
  double result = 0.001;
  while (result < input) {
    if (result < input) {
      result *= 2;
    }
    if (result < input) {
      result = result / 2 * 5;
    }
    if (result < input) {
      result *= 2;
    }
  }
  return result;
}

GridRenderer::GridRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  grid_size_(100) {
}

void GridRenderer::InitializeGL() {
  Scene::Ptr scene = GetScene();

  ResourceManager::Ptr resources = GetResources();
  StockResources stock(resources);

  // Base layer material
  base_material_ = stock.NewMaterial(StockResources::kUniformColorNoLighting);
  base_material_->SetDepthWrite(false);
  base_material_->SetTwoSided(true);
  base_material_->SetParam("color", 0.1, 0.1, 0.1, 1.0);

  // Grid material
  grid_material_ = stock.NewMaterial(StockResources::kUniformColorNoLighting);
  grid_material_->SetParam("color", 0.75, 0.75, 0.75, 1.0);

  // Depth write material
  depth_write_material_ = resources->MakeMaterial(base_material_->Shader());
  depth_write_material_->SetColorWrite(false);
  depth_write_material_->SetTwoSided(true);

  // Geometry
  base_geom_ = resources->MakeGeometry();
  grid_geom_ = resources->MakeGeometry();

  UpdateGeometry();

  mesh_ = scene->MakeMesh(GetBaseNode());
  mesh_->Add(base_geom_, base_material_);
  mesh_->Add(grid_geom_, grid_material_);
  mesh_->Add(base_geom_, depth_write_material_);
}

void GridRenderer::RenderBegin() {
  // Calculate camera distance from grid
  CameraNode* camera = GetViewport()->GetCamera();
  const double distance =
    (camera->Translation() - camera->GetLookAt()).length();

  const double grid_spacing = RoundTo125(distance / 10);
  mesh_->SetScale(grid_spacing, grid_spacing, 1);
}

void GridRenderer::UpdateGeometry() {
  gdata_.gl_mode = GL_LINES;

  const double spacing = 1.0;
  const double xy_span = spacing * grid_size_;
  const double xy_min = -xy_span / 2;
  const double xy_max = xy_span / 2;

  // X-axis grid lines
  for (int i = 0; i <= grid_size_; ++i) {
    const double x0 = xy_min;
    const double x1 = xy_max;
    const double y = xy_min + i * spacing;
    const double z = 0;
    gdata_.vertices.emplace_back(x0, y, z);
    gdata_.vertices.emplace_back(x1, y, z);
  }

  // Y-axis grid lines
  for (int i = 0; i <= grid_size_; ++i) {
    const double y0 = xy_min;
    const double y1 = xy_max;
    const double x = xy_min + i * spacing;
    const double z = 0;
    gdata_.vertices.emplace_back(x, y0, z);
    gdata_.vertices.emplace_back(x, y1, z);
  }

  grid_geom_->Load(gdata_);

  // Calculate the geometry for the base and depth write layers.
  GeometryData bdata;
  bdata.gl_mode = GL_TRIANGLE_STRIP;
  bdata.vertices = {
    QVector3D(xy_min, xy_min, 0),
    QVector3D(xy_max, xy_min, 0),
    QVector3D(xy_min, xy_max, 0),
    QVector3D(xy_max, xy_max, 0),
  };
  base_geom_->Load(bdata);
}

}  // namespace sceneview
