// Copyright [2015] Albert Huang

#include "sceneview/font_resource.hpp"

#include <algorithm>

#include <QFontMetrics>
#include <QOpenGLTexture>
#include <QPainter>
#include <QPixmap>

namespace sv {

struct FontResource::Priv {
  int num_rows;
  int num_cols;
  int block_size;

  CharData char_data[256];

  std::shared_ptr<QOpenGLTexture> texture;
};

FontResource::~FontResource() { delete p_; }

const std::shared_ptr<QOpenGLTexture>& FontResource::Texture() {
  return p_->texture;
}

const FontResource::CharData& FontResource::GetCharData(int c) {
  return p_->char_data[c];
}

FontResource::Ptr FontResource::Create(const QFont& font) {
  return Ptr(new FontResource(font));
}

FontResource::FontResource(const QFont& font) : p_(new Priv) { Build(font); }

void FontResource::Build(const QFont& base_font) {
  // Build a texture with a grid of characters.
  p_->num_cols = 16;
  p_->num_rows = 16;

  // The nominal size of each character.
  const int pixel_size = 64;

  // Override the font pixel size.
  QFont font(base_font);
  font.setPixelSize(pixel_size);
  QFontMetrics font_metrics(font);

  // Calculate the bounding rectangles for each character, and the maximum
  // extents that a character might deviate from its anchor point.
  p_->block_size = 0;
  int leftmost = 0;
  int topmost = 0;
  int rightmost = 0;
  QRect bounding_rects[256];
  int bottommost = 0;
  for (int c = 0; c < 256; ++c) {
    const QRect bounding_rect = font_metrics.boundingRect(c);
    bounding_rects[c] = bounding_rect;
    leftmost = std::min(leftmost, bounding_rect.left());
    topmost = std::min(topmost, bounding_rect.top());
    rightmost = std::max(rightmost, bounding_rect.right());
    bottommost = std::max(bottommost, bounding_rect.bottom());
  }

  // Use the bounding data to calculate how big each grid cell should be, and
  // subsequently the size of the texture.
  p_->block_size = std::max(bottommost - topmost, rightmost - leftmost);
  const int width = p_->num_cols * p_->block_size + rightmost;
  const int height = p_->num_rows * p_->block_size + bottommost;

  // Initialize the texture to completely transparent and prepare to draw.
  QPixmap pixmap(width, height);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setFont(font);
  painter.setPen(Qt::white);
  QBrush brush(Qt::white);

  // Draw each character and compute its draw data.
  for (int c = 0; c < 256; ++c) {
    const int x = p_->block_size * (c % p_->num_cols);
    const int y = p_->block_size * (c / p_->num_cols + 1);
    painter.drawText(x, y, QString(QChar(c)));

    const QRect& bounding_rect = bounding_rects[c];
    const int x0 = x + bounding_rect.left();
    const int y0 = y + bounding_rect.top();
    const int x1 = x + bounding_rect.right() + 1;
    const int y1 = y + bounding_rect.bottom() + 1;
    p_->char_data[c].u0 = static_cast<float>(x0) / width;
    p_->char_data[c].v0 = static_cast<float>(y0) / height;
    p_->char_data[c].u1 = static_cast<float>(x1) / width;
    p_->char_data[c].v1 = static_cast<float>(y1) / height;
    p_->char_data[c].width_to_height =
        static_cast<float>(font_metrics.width(c)) / p_->block_size;

    p_->char_data[c].x0 =
        bounding_rect.left() / static_cast<float>(p_->block_size);
    p_->char_data[c].y0 =
        bounding_rect.top() / static_cast<float>(p_->block_size);
    p_->char_data[c].x1 =
        bounding_rect.right() / static_cast<float>(p_->block_size);
    p_->char_data[c].y1 =
        bounding_rect.bottom() / static_cast<float>(p_->block_size);
  }

  const QImage image = pixmap.toImage();
  const QImage converted = image.convertToFormat(QImage::Format_RGBA8888);
  p_->texture.reset(new QOpenGLTexture(converted));
  p_->texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  p_->texture->setMagnificationFilter(QOpenGLTexture::Linear);
}

}  // namespace sv
