#include "hud_renderer.hpp"

#include <QColor>
#include <QPainter>

#include <sceneview/draw_group.hpp>
#include <sceneview/viewport.hpp>

using sv::StockResources;
using sv::TextBillboard;

namespace vis_examples {

HudRenderer::HudRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent) {
}

void HudRenderer::InitializeGL() {
  StockResources stock(GetResources());

  geom_ = GetResources()->MakeGeometry();
  material_ = stock.NewMaterial(StockResources::kPerVertexColorNoLighting);

  // Draw fat thick lines.
  material_->SetLineWidth(5.0f);

  sv::Scene::Ptr scene = GetScene();
  sv::GroupNode* group_node = scene->MakeGroup(GetBaseNode());

  draw_node_ = scene->MakeDrawNode(group_node, geom_, material_);
  connect(GetViewport(), &sv::Viewport::resized,
      [this]() { geom_dirty_ = true; });

  text_billboard_ =
    TextBillboard::Create(GetViewport(), group_node);

  text_billboard_->SetFont(QFont("Arial"));
  text_billboard_->SetLineHeight(25);
  text_billboard_->SetTextColor(QColor(255, 255, 255));
  text_billboard_->SetBackgroundColor(QColor(50, 50, 50));
  text_billboard_->SetAlignment(TextBillboard::kLeft, TextBillboard::kTop);
  text_billboard_->SetYDirection(TextBillboard::kPositive);
  text_billboard_->Node()->SetTranslation(0, 0, 0);

  sv::DrawGroup* hud_group = scene->GetDrawGroup("HUD");
  scene->SetDrawGroup(group_node, hud_group);

  UpdateGeometry();
}

void HudRenderer::ShutdownGL() {
  text_billboard_.reset();
}

void HudRenderer::RenderBegin() {
  if (geom_dirty_) {
    UpdateGeometry();
    geom_dirty_ = false;
  }
}

void HudRenderer::UpdateGeometry() {
  const int width = GetViewport()->width();
  const int height = GetViewport()->height();

  const float x0 = 0;
  const float y0 = 0;
  const float x1 = width / 2;
  const float y1 = height / 4;
  gdata_.gl_mode = GL_LINES;
  gdata_.vertices = {
    { x0, y0, -0.1 },
    { x1, y1, -0.1 },
    { x1, y0, -0.1 },
    { x0, y1, -0.1 },
  };
  gdata_.diffuse = {
    { 1.0, 0., 0., 1. },
    { 0.0, 1., 0., 1. },
    { 0.0, 0., 1., 1. },
    { 1., 1., 0., 1. },
  };

  geom_->Load(gdata_);

  QString text = QString(
      "HUD example\n"
      "Text that stays attached to the window");
  text_billboard_->SetText(text);
}

}  // namespace vis_examples
