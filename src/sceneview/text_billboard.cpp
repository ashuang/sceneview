// Copyright [2015] Albert Huang

#include "sceneview/text_billboard.hpp"

#include <QColor>
#include <algorithm>

#include "sceneview/camera_node.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/draw_context.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/stock_resources.hpp"
#include "sceneview/viewport.hpp"

namespace sv {

/**
 * Use a custom drawable for the text billboard so that we can report a
 * different bounding box than would be expected by the geometry.
 */
class TextBillboardDrawable : public Drawable {
  public:
    TextBillboardDrawable(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material) :
      Drawable(geometry, material) {}

    // The bounding box depends on the camera, so recompute it every time
    // it's requested.
    const AxisAlignedBox& BoundingBox() override {
      return aabb_;
    }

    void SetBoundingBox(const AxisAlignedBox& box) {
      aabb_ = box;
    }

  private:
    AxisAlignedBox aabb_;
};

TextBillboard::Ptr TextBillboard::Create(Viewport* viewport, GroupNode* parent) {
  return Ptr(new TextBillboard(viewport, parent));
}

TextBillboard::TextBillboard(Viewport* viewport,
    GroupNode* parent) :
  viewport_(viewport),
  resources_(viewport_->GetResources()),
  scene_(viewport_->GetScene()),
  parent_(parent),
  v_align_(kVCenter),
  h_align_(kHCenter),
  line_height_(0.1),
  margin_top_(0.2),
  margin_left_(0.2),
  margin_bottom_(0.2),
  margin_right_(0.2),
  qfont_("Helvetica"),
  text_() {
  // Implementation details
  // The text display is rendered as two layered geometries:
  // 1. Background layer (a single rectangle)
  // 2. Text layer (one textured quad per character)

  StockResources stock(resources_);

  // Create the background geometry and material
  ShaderResource::Ptr billboard_uniform_color =
    stock.Shader(StockResources::kBillboardUniformColor);
  bg_material_ = resources_->MakeMaterial(billboard_uniform_color);
  bg_material_->SetParam("color", 0.0, 0.0, 0.0, 0.0);
  bg_material_->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  bg_material_->SetTwoSided(true);

  // Create geometry for the background and the cap
  rect_geom_ = resources_->MakeGeometry();

  // Create the text geometry and material
  text_material_ = resources_->MakeMaterial(
      stock.Shader(StockResources::kBillboardTextured));
  text_material_->SetTwoSided(true);
  text_material_->SetDepthFunc(GL_LEQUAL);
  text_material_->SetParam("text_color", 1.0, 1.0, 1.0, 1.0);
  text_material_->SetBlend(true);
  text_material_->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  text_geom_ = resources_->MakeGeometry();

  node_ = scene_->MakeGroup(parent_);
  draw_node_ = scene_->MakeDrawNode(node_);

  // Use a custom Drawable to correctly compute the node bounding box.
  bg_drawable_ = new TextBillboardDrawable(rect_geom_, bg_material_);
  text_drawable_ = new TextBillboardDrawable(text_geom_, text_material_);
  draw_node_->Add(Drawable::Ptr(bg_drawable_));
  draw_node_->Add(Drawable::Ptr(text_drawable_));
}

TextBillboard::~TextBillboard() {
  scene_->DestroyNode(node_);
  font_resource_.reset();
  text_geom_.reset();
  text_material_.reset();
}

void TextBillboard::SetText(const QString& text) {
  text_ = text.toStdString();
  Recompute();
}

void TextBillboard::SetLineHeight(float height) {
  line_height_ = height;
  Recompute();
}

void TextBillboard::SetFont(const QFont& font) {
  qfont_ = font;
  Recompute();
}

void TextBillboard::SetTextColor(const QColor& color) {
  text_material_->SetParam("text_color",
      color.redF(),
      color.greenF(),
      color.blueF(),
      color.alphaF());
}

void TextBillboard::SetBackgroundColor(const QColor& color) {
  bg_material_->SetParam("color",
      color.redF(),
      color.greenF(),
      color.blueF(),
      color.alphaF());
  bg_material_->SetBlend(color.alphaF() < 1);
}

void TextBillboard::SetAlignment(HAlignment horizontal,
    VAlignment vertical) {
  h_align_ = horizontal;
  v_align_ = vertical;
  Recompute();
}

void TextBillboard::SetYDirection(YDirection direction) {
  if (direction != kPositive && direction != kNegative) {
    throw std::invalid_argument("Invalid direction");
  }
  y_dir_ = direction;
}

void TextBillboard::Recompute() {
  font_resource_ = resources_->Font(qfont_);

  GeometryData gdata;
  gdata.gl_mode = GL_TRIANGLES;

  const QVector3D normal(0, 0, 1);

  const float x_dir = 1;
  const float y_step = y_dir_ * line_height_;

  float cursor_x = 0;
  float cursor_y = 0;
  float min_x = 0;
  float max_x = 0;
  float min_y = 0;
  float max_y = 0;

  // Place each character in the text string and compute both its vertex
  // positions and texture coordinates.
  int base_vertex = 0;
  for (unsigned ch_index = 0; ch_index < text_.size(); ++ch_index) {
    const char ch = text_[ch_index];
    if (ch == '\n') {
      cursor_x = 0;
      cursor_y += y_step;
      continue;
    }

    const FontResource::CharData& cdata = font_resource_->GetCharData(ch);
    const float x0 = cursor_x + x_dir * cdata.x0 * line_height_;
    const float x1 = cursor_x + x_dir * cdata.x1 * line_height_;
    const float y0 = cursor_y + y_step * cdata.y0;
    const float y1 = cursor_y + y_step * cdata.y1;

    min_x = std::min(min_x, std::min(x0, x1));
    min_y = std::min(min_y, std::min(y0, y1));
    max_x = std::max(max_x, std::max(x0, x1));
    max_y = std::max(max_y, std::max(y0, y1));

    gdata.vertices.emplace_back(x0, y0, 0);
    gdata.vertices.emplace_back(x1, y0, 0);
    gdata.vertices.emplace_back(x1, y1, 0);
    gdata.vertices.emplace_back(x0, y1, 0);

    gdata.normals.push_back(normal);
    gdata.normals.push_back(normal);
    gdata.normals.push_back(normal);
    gdata.normals.push_back(normal);

    gdata.indices.push_back(base_vertex);
    gdata.indices.push_back(base_vertex + 3);
    gdata.indices.push_back(base_vertex + 1);

    gdata.indices.push_back(base_vertex + 3);
    gdata.indices.push_back(base_vertex + 2);
    gdata.indices.push_back(base_vertex + 1);

    gdata.tex_coords_0.emplace_back(cdata.u0, cdata.v0);
    gdata.tex_coords_0.emplace_back(cdata.u1, cdata.v0);
    gdata.tex_coords_0.emplace_back(cdata.u1, cdata.v1);
    gdata.tex_coords_0.emplace_back(cdata.u0, cdata.v1);

    base_vertex += 4;

    cursor_x += cdata.width_to_height * line_height_;
  }

  // Align vertices relative to anchor point.
  float x_offset = 0;
  float y_offset = 0;
  switch (h_align_) {
    case kLeft:
      x_offset = -min_x;
      break;
    case kHCenter:
      x_offset = -(max_x + min_x) / 2;
      break;
    case kRight:
      x_offset = -max_x;
      break;
    default:
      break;
  }

  if (y_dir_ == kPositive) {
    switch (v_align_) {
      case kTop:
        y_offset = -min_y;
        break;
      case kVCenter:
        y_offset = -(max_y + min_y) / 2;
        break;
      case kBottom:
        y_offset = -max_y;
        break;
      default:
        break;
    }
  } else {
    switch (v_align_) {
      case kTop:
        y_offset = -max_y;
        break;
      case kVCenter:
        y_offset = -(max_y + min_y) / 2;
        break;
      case kBottom:
        y_offset = -min_y;
        break;
      default:
        break;
    }
  }

  // Calculate bounding box margins to arrive at a final vertex offset
  const float margin_left_actual = margin_left_ * line_height_;
  const float margin_right_actual = margin_right_ * line_height_;
  const float margin_top_actual = margin_top_ * line_height_;
  const float margin_bottom_actual = margin_bottom_ * line_height_;

  const QVector3D offset(x_offset + margin_left_actual,
      y_offset + margin_bottom_actual, 0);

  for (auto& vertex : gdata.vertices) {
    vertex += offset;
  }

  // Done calculating the text geometry. Load it into the geometry resource.
  text_geom_->Load(gdata);

  // Make sure the material is configured to use the active font.
  text_material_->AddTexture("texture", font_resource_->Texture());

  // Calculate the geometry for the background and depth write layers.
  const float x0 = min_x + x_offset;
  const float y0 = min_y + y_offset;
  const float x1 = max_x + x_offset + margin_left_actual + margin_right_actual;
  const float y1 = max_y + y_offset + margin_bottom_actual + margin_top_actual;
  GeometryData rdata;
  rdata.gl_mode = GL_TRIANGLE_STRIP;
  rdata.vertices = {
    QVector3D(x0, y0, 0),
    QVector3D(x1, y0, 0),
    QVector3D(x0, y1, 0),
    QVector3D(x1, y1, 0),
  };
  rect_geom_->Load(rdata);

  // Compute a bounding box that will be valid for all camera poses. This box
  // will generally be much bigger than actually needed, but should always
  // bound the billboard.
  //
  // We could compute the true bounding box at render time, but that would be
  // more computationally expensive and requires a few matrix multiplications
  // and lookups every render cycle.
  const float max_coord = std::max(x1, y1);
  AxisAlignedBox box(QVector3D(-max_coord, -max_coord, -max_coord),
      QVector3D(max_coord, max_coord, max_coord));

  bg_drawable_->SetBoundingBox(box);
  text_drawable_->SetBoundingBox(box);
}

}  // namespace sv
