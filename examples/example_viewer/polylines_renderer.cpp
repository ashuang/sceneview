#include "polylines_renderer.hpp"

#include <cmath>

#include <sceneview/stock_resources.hpp>

using sv::StockResources;

namespace vis_examples {

/**
 * Demonstrates building a draw node with custom geometry that gets dynamically
 * updated over time.
 */
PolylinesRenderer::PolylinesRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  start_time_(QTime::currentTime()),
  angle_(0) {
}

void PolylinesRenderer::InitializeGL() {
  StockResources stock(GetResources());

  geom_ = GetResources()->MakeGeometry();
  material_ = stock.NewMaterial(StockResources::kPerVertexColorNoLighting);

  // Draw fat thick lines.
  material_->SetLineWidth(10.0f);

  draw_node_ = GetScene()->MakeDrawNode(GetBaseNode(), geom_, material_);

  UpdateGeometry();
}

void PolylinesRenderer::RenderBegin() {
  UpdateGeometry();
}

void PolylinesRenderer::UpdateGeometry() {
  gdata_.gl_mode = GL_LINE_STRIP;
  gdata_.vertices.clear();
  gdata_.diffuse.clear();

  const double radius = 6.0;
  const double elapsed = start_time_.restart() / 1000.;
  const double speed = 1.0;
  angle_ += elapsed * speed;
  const double arc_length = 75 * M_PI / 180;
  const double step = 2 * M_PI / 180;
  const int num_steps = arc_length / step;

  for (int step_num = 0; step_num < num_steps; ++step_num) {
    const double theta0 = angle_ + step * step_num;
    const double theta1 = theta0 + step;
    const double cos0 = cos(theta0);
    const double sin0 = sin(theta0);
    const double cos1 = cos(theta1);
    const double sin1 = sin(theta1);
    const double x0 = radius * cos0;
    const double y0 = radius * sin0;
    const double x1 = radius * cos1;
    const double y1 = radius * sin1;
    gdata_.vertices.emplace_back(x0, y0, -0.05);
    gdata_.vertices.emplace_back(x1, y1, -0.05);

    // Since we chose the material kPerVertexColorNoLighting, specify the color
    // in the per-vertex geometry data.
    //
    // If we used a different material, then color would be specified some
    // other way.
    const float r0 = cos0 * 0.5 + 0.5;
    const float g0 = sin0 * 0.5 + 0.5;
    const float r1 = cos1 * 0.5 + 0.5;
    const float g1 = sin1 * 0.5 + 0.5;
    gdata_.diffuse.emplace_back(r0, g0, 0, 1);
    gdata_.diffuse.emplace_back(r1, g1, 0, 1);
  }

  geom_->Load(gdata_);
}

}  // namespace vis_examples
