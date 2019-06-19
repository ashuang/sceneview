#include "stencil_renderer.hpp"

#include <cmath>

#include <sceneview/scene_node.hpp>
#include <sceneview/stock_resources.hpp>

namespace vis_examples {

StencilRenderer::StencilRenderer(const QString& name, QObject* parent)
    : sv::Renderer(name, parent) {}

void StencilRenderer::InitializeGL() {
  sv::Scene::Ptr scene = GetScene();
  sv::ResourceManager::Ptr resources = GetResources();
  sv::GroupNode* base_node = GetBaseNode();
  sv::StockResources stock(resources);

  // Setup stencil buffer configuration. This stencil configuration can get
  // attached to a material to configure the material to use the stencil
  // buffer.
  sv::StencilSettings stencil;

  // Stencil test passes if stencil buffer has value zero
  stencil.front.func = GL_EQUAL;
  stencil.front.func_ref = 0;
  stencil.front.func_mask = 0xff;

  // When the stencil test passes, increment the stencil buffer value
  stencil.front.sfail = GL_KEEP;
  stencil.front.dpfail = GL_KEEP;
  stencil.front.dppass = GL_INCR;
  stencil.front.mask = 0xff;

  // treat back faces the same as the front
  stencil.back = stencil.front;

  // Create a common geometry - a single square made up of two triangles
  sv::GeometryData gdata;
  gdata.gl_mode = GL_TRIANGLES;
  gdata.vertices = {
    QVector3D(0, 0, 0),
    QVector3D(1, 0., 0.),
    QVector3D(1, 1, 0.),
    QVector3D(0., 1, 0.)
  };
  gdata.tex_coords_0 = {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 }
  };
  gdata.indices = { 0, 1, 2, 0, 2, 3 };
  geom_ = resources->MakeGeometry();
  geom_->Load(gdata);

  // Node A
  material_a_ = stock.NewMaterial(sv::StockResources::kUniformColorNoLighting);
  material_a_->SetParam(sv::kColor, 1.0, 1.0, 0.0, 1.0);
  material_a_->SetStencil(stencil);
  material_a_->SetTwoSided(true);

  // Override the draw order of node a and force it to draw first.
  sv::DrawNode* node_a = GetScene()->MakeDrawNode(base_node, geom_, material_a_);
  node_a->SetTranslation(3, 3, -0.1);
  node_a->SetScale(2, 2, 1);

  // Node B
  material_b_ = stock.NewMaterial(sv::StockResources::kUniformColorNoLighting);
  material_b_->SetParam(sv::kColor, 0.0, 1.0, 1.0, 1.0);
  material_b_->SetStencil(stencil);
  material_b_->SetTwoSided(true);

  sv::DrawNode* node_b = GetScene()->MakeDrawNode(base_node, geom_, material_b_);
  node_b->SetTranslation(2, 2, -0.1);
  node_b->SetScale(2, 2, 1);

  // Set the draw order of node B to force it to draw after node A. Combined
  // with the stencil settings, this means that node B will have one quadrant
  // not drawn. If we wanted to draw the full node B square instead of the node
  // A square, then we could change the draw order of node B such that it has a
  // lower value than node A, thereby forcing it to draw before node A.
  node_b->SetDrawOrder(1);
}

}  // namespace vis_examples
