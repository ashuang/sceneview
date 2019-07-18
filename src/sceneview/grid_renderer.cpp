// Copyright [2015] Albert Huang

#include "sceneview/grid_renderer.hpp"

#include "sceneview/camera_node.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/stock_resources.hpp"
#include "sceneview/viewport.hpp"

namespace sv {

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

struct GridRenderer::Priv {
  MaterialResource::Ptr base_material;
  MaterialResource::Ptr grid_material;
  MaterialResource::Ptr depth_write_material;

  GeometryResource::Ptr grid_geom;
  GeometryResource::Ptr base_geom;
  DrawNode* draw_node;

  int grid_size;

  GeometryData gdata;
};

GridRenderer::GridRenderer(const QString& name, QObject* parent)
    : Renderer(name, parent), p_(new Priv) {
  p_->grid_size = 100;
}

GridRenderer::~GridRenderer() { delete p_; }

void GridRenderer::InitializeGL() {
  Scene::Ptr scene = GetScene();

  ResourceManager::Ptr resources = GetResources();
  StockResources stock(resources);

  // Approach
  // The grid is rendered as three layered geometries:
  // 1. Base layer (a single rectangle)
  // 2. Grid layer (the actual grid lines)
  // 3. Depth write layer (a single rectangle)
  //
  // Layers 1 and 3 have identical geometries, but differ in their material
  // properties:
  // - Layer 1: depth write disabled, color write enabled
  // - Layer 3: depth write enabled, color write disabled
  //
  // The purpose is to allow the grid lines to be rendered at the exact same
  // depth as the base layer without having to fudge the depth test or mess
  // with things like glPolygonOffset().

  // Base layer material
  p_->base_material = stock.NewMaterial(StockResources::kUniformColorNoLighting);
  p_->base_material->SetDepthWrite(false);
  p_->base_material->SetTwoSided(true);
  p_->base_material->SetParam("color", 0.1, 0.1, 0.1, 1.0);

  // Grid material
  p_->grid_material = stock.NewMaterial(StockResources::kUniformColorNoLighting);
  p_->grid_material->SetParam("color", 0.75, 0.75, 0.75, 1.0);

  // Depth write material
  p_->depth_write_material = resources->MakeMaterial(p_->base_material->Shader());
  p_->depth_write_material->SetColorWrite(false);
  p_->depth_write_material->SetTwoSided(true);

  // Geometry
  p_->base_geom = resources->MakeGeometry();
  p_->grid_geom = resources->MakeGeometry();

  UpdateGeometry();

  p_->draw_node = scene->MakeDrawNode(GetBaseNode());
  p_->draw_node->Add(p_->base_geom, p_->base_material);
  p_->draw_node->Add(p_->grid_geom, p_->grid_material);
  p_->draw_node->Add(p_->base_geom, p_->depth_write_material);
}

void GridRenderer::RenderBegin() {
  // Calculate camera distance from grid
  CameraNode* camera = GetViewport()->GetCamera();
  const double distance =
      (camera->Translation() - camera->GetLookAt()).length();

  const double grid_spacing = RoundTo125(distance / 10);
  p_->draw_node->SetScale(grid_spacing, grid_spacing, 1);
}

void GridRenderer::UpdateGeometry() {
  p_->gdata.gl_mode = GL_LINES;

  const double spacing = 1.0;
  const double xy_span = spacing * p_->grid_size;
  const double xy_min = -xy_span / 2;
  const double xy_max = xy_span / 2;

  // X-axis grid lines
  for (int i = 0; i <= p_->grid_size; ++i) {
    const double x0 = xy_min;
    const double x1 = xy_max;
    const double y = xy_min + i * spacing;
    const double z = 0;
    p_->gdata.vertices.emplace_back(x0, y, z);
    p_->gdata.vertices.emplace_back(x1, y, z);
  }

  // Y-axis grid lines
  for (int i = 0; i <= p_->grid_size; ++i) {
    const double y0 = xy_min;
    const double y1 = xy_max;
    const double x = xy_min + i * spacing;
    const double z = 0;
    p_->gdata.vertices.emplace_back(x, y0, z);
    p_->gdata.vertices.emplace_back(x, y1, z);
  }

  p_->grid_geom->Load(p_->gdata);

  // Calculate the geometry for the base and depth write layers.
  GeometryData bdata;
  bdata.gl_mode = GL_TRIANGLE_STRIP;
  bdata.vertices = {
      QVector3D(xy_min, xy_min, 0),
      QVector3D(xy_max, xy_min, 0),
      QVector3D(xy_min, xy_max, 0),
      QVector3D(xy_max, xy_max, 0),
  };
  p_->base_geom->Load(bdata);
}

}  // namespace sv
