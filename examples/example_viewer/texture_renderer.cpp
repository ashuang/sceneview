#include "texture_renderer.hpp"

#include <cmath>

#include <QOpenGLTexture>

#include <sceneview/scene_node.hpp>
#include <sceneview/stock_resources.hpp>

using sv::GroupNode;
using sv::Drawable;
using sv::DrawNode;
using sv::ParamWidget;
using sv::Renderer;
using sv::ResourceManager;
using sv::Scene;
using sv::StockResources;
using sv::SceneNode;

namespace vis_examples {

TextureRenderer::TextureRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  start_time_(QTime::currentTime()) {
}

void TextureRenderer::InitializeGL() {
  StockResources stock(GetResources());

  geom_ = GetResources()->MakeGeometry();
  material_ = stock.NewMaterial(StockResources::kTextureUniformColorNoLighting);
  material_->SetParam(sv::kColor, 1.0, 1.0, 1.0, 1.0);
  draw_node_ = GetScene()->MakeDrawNode(GetBaseNode(), geom_, material_);

  const QVector3D p0 = { 2, 1, -0.1 };
  const float dx = 2;
  const float dy = 2;
  gdata_.gl_mode = GL_TRIANGLES;
  gdata_.vertices = {
    p0,
    p0 + QVector3D(dx, 0., 0.),
    p0 + QVector3D(dx, dy, 0.),
    p0 + QVector3D(0., dy, 0.)
  };
  gdata_.tex_coords_0 = {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 }
  };
  gdata_.indices = { 0, 1, 2, 0, 2, 3 };
  geom_->Load(gdata_);
  material_->SetTwoSided(true);

  MakeTexture();
}

void TextureRenderer::RenderBegin() {
  MakeTexture();
}

void TextureRenderer::MakeTexture() {
  const double elapsed = start_time_.elapsed() / 1000.;
  const float tx0 = elapsed;
  const float ty0 = elapsed;

  QImage image(tex_width_, tex_height_, QImage::Format_RGB888);
  for (int y = 0; y < tex_height_; ++y) {
    uchar* row = image.scanLine(y);
    for (int x = 0; x < tex_width_; ++x) {
      const float tx = static_cast<float>(x) / tex_width_;
      const float ty = static_cast<float>(y) / tex_height_;
      const int red = 255 * (cos(tx * 50 + tx0) * 0.5 + 0.5);
      const int green = 255 * (sin(ty * 50 + ty0) * 0.5 + 0.5);
      const int blue = 255 * (cos((tx + ty) * 50) * 0.5 + 0.5);
      row[x * 3 + 0] = red;
      row[x * 3 + 1] = green;
      row[x * 3 + 2] = blue;
    }
  }
  texture_.reset(new QOpenGLTexture(image));
  texture_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture_->setMagnificationFilter(QOpenGLTexture::Linear);
  texture_->setWrapMode(QOpenGLTexture::ClampToEdge);

  material_->AddTexture(sv::kTexture0, texture_);
}

}  // namespace vis_examples
