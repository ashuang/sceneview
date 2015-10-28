// Copyright [2015] Albert Huang

#include "sceneview/font_resource.hpp"

#include <algorithm>

#include <QFontMetrics>
#include <QOpenGLTexture>
#include <QPixmap>
#include <QPainter>

namespace sv {

FontResource::Ptr FontResource::Create(const QFont& font) {
  return Ptr(new FontResource(font));
}

FontResource::FontResource(const QFont& font) {
  Build(font);
}

void FontResource::Build(const QFont& base_font) {
  // Build a texture with a grid of characters.
  num_cols_ = 16;
  num_rows_ = 16;

  // The nominal size of each character.
  const int pixel_size = 64;

  // Override the font pixel size.
  QFont font(base_font);
  font.setPixelSize(pixel_size);
  QFontMetrics font_metrics(font);

  // Calculate the bounding rectangles for each character, and the maximum
  // extents that a character might deviate from its anchor point.
  block_size_ = 0;
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
  block_size_ = std::max(bottommost - topmost, rightmost - leftmost);
  const int width = num_cols_ * block_size_ + rightmost;
  const int height = num_rows_ * block_size_ + bottommost;

  // Initialize the texture to completely transparent and prepare to draw.
  QPixmap pixmap(width, height);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setFont(font);
  painter.setPen(Qt::white);
  QBrush brush(Qt::white);

  // Draw each character and compute its draw data.
  for (int c = 0; c < 256; ++c) {
    const int x = block_size_ * (c % num_cols_);
    const int y = block_size_ * (c / num_cols_ + 1);
    painter.drawText(x, y, QString(QChar(c)));

    const QRect& bounding_rect = bounding_rects[c];
    const int x0 = x + bounding_rect.left();
    const int y0 = y + bounding_rect.top();
    const int x1 = x + bounding_rect.right() + 1;
    const int y1 = y + bounding_rect.bottom() + 1;
    char_data_[c].u0 = static_cast<float>(x0) / width;
    char_data_[c].v0 = static_cast<float>(y0) / height;
    char_data_[c].u1 = static_cast<float>(x1) / width;
    char_data_[c].v1 = static_cast<float>(y1) / height;
    char_data_[c].width_to_height = static_cast<float>(
        font_metrics.width(c)) / block_size_;

    char_data_[c].x0 = bounding_rect.left() / static_cast<float>(block_size_);
    char_data_[c].y0 = bounding_rect.top() / static_cast<float>(block_size_);
    char_data_[c].x1 = bounding_rect.right() / static_cast<float>(block_size_);
    char_data_[c].y1 = bounding_rect.bottom() / static_cast<float>(block_size_);
  }

  const QImage image = pixmap.toImage();
  const QImage converted = image.convertToFormat(QImage::Format_RGBA8888);
  texture_.reset(new QOpenGLTexture(converted));
  texture_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture_->setMagnificationFilter(QOpenGLTexture::Linear);
}

}  // namespace sv
