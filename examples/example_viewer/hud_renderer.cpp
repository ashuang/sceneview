#include "hud_renderer.hpp"

#include <QColor>

#include <sceneview/draw_group.hpp>
#include <sceneview/viewport.hpp>

using sv::StockResources;

namespace vis_examples {

HudRenderer::HudRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  timer_() {
  timer_.start();
}

void HudRenderer::InitializeGL() {
  StockResources stock(GetResources());

  geom_ = GetResources()->MakeGeometry();
  material_ = stock.NewMaterial(StockResources::kPerVertexColorNoLighting);

  // Draw fat thick lines.
  material_->SetLineWidth(5.0f);
  material_->SetDepthTest(false);
//  material_->SetDepthWrite(false);

  sv::Scene::Ptr scene = GetScene();
  draw_node_ = scene->MakeDrawNode(GetBaseNode(), geom_, material_, "hudnode");

  sv::DrawGroup* hud_group = scene->GetDrawGroup("HUD");
  scene->SetDrawGroup(draw_node_, hud_group);

  UpdateGeometry();
}

void HudRenderer::ShutdownGL() {
}

void HudRenderer::RenderBegin() {
  UpdateGeometry();
}

void HudRenderer::UpdateGeometry() {
  const int width = GetViewport()->width();
  const int height = GetViewport()->height();

  const float x0 = width / 2;
  const float y0 = height / 2;
  const float x1 = width;
  const float y1 = height;
  gdata_.gl_mode = GL_LINES;
  gdata_.vertices = {
    { x0, y0, 0 },
    { x1, y1, 0 },
    { x1, y0, 0 },
    { x0, y1, 0 },
  };
  gdata_.diffuse = {
    { 1.0, 0., 0., 1. },
    { 1.0, 0., 0., 1. },
    { 0.0, 1., 0., 1. },
    { 0.0, 1., 0., 1. },
  };

  geom_->Load(gdata_);
}

}  // namespace vis_examples
