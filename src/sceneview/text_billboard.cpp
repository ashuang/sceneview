// Copyright [2015] Albert Huang

#include "sceneview/text_billboard.hpp"

#include <QColor>
#include <algorithm>

#include "sceneview/group_node.hpp"
#include "sceneview/mesh_node.hpp"
#include "sceneview/stock_resources.hpp"

namespace sceneview {

TextBillboard::Ptr TextBillboard::Create(const ResourceManager::Ptr& resources,
    const Scene::Ptr& scene,
    GroupNode* parent) {
  return Ptr(new TextBillboard(resources, scene, parent));
}

TextBillboard::TextBillboard(const ResourceManager::Ptr& resources,
    const Scene::Ptr& scene,
    GroupNode* parent) :
  resources_(resources),
  scene_(scene),
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
  // The text display is rendered as three layered geometries:
  // 1. Background layer (a single rectangle)
  // 2. Text layer (one textured quad per character)
  // 3. Depth write layer (a single rectangle)
  //
  // Layers 1 and 3 have identical geometries, but differ in their material
  // properties:
  // - Layer 1: depth write disabled, color write enabled
  // - Layer 3: depth write enabled, color write disabled
  //
  // The purpose is to allow the text to be rendered at the exact same depth as
  // the background material without having to fudge the depth test or mess with
  // things like glPolygonOffset().

  StockResources stock(resources);

  // Create the background geometry and material
  ShaderResource::Ptr billboard_uniform_color =
    stock.Shader(StockResources::kBillboardUniformColor);
  bg_material_ = resources->MakeMaterial(billboard_uniform_color);
  bg_material_->SetDepthWrite(false);
  bg_material_->SetParam("color", 0.0, 0.0, 0.0, 0.0);

  // Create geometry for the background and the cap
  rect_geom_ = resources->MakeGeometry();

  // Create the text geometry and material
  text_material_ = resources->MakeMaterial(
      stock.Shader(StockResources::kBillboardTextured));
  text_material_->SetTwoSided(true);
  text_material_->SetParam("text_color", 1.0, 1.0, 1.0, 1.0);

  // Create the depth write material
  depth_write_material_ = resources->MakeMaterial(billboard_uniform_color);
  depth_write_material_->SetColorWrite(false);

  text_geom_ = resources->MakeGeometry();

  node_ = scene_->MakeGroup(parent_);
  mesh_ = scene_->MakeMesh(node_);

  mesh_->Add(rect_geom_, bg_material_);
  mesh_->Add(text_geom_, text_material_);
  mesh_->Add(rect_geom_, depth_write_material_);
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
}

void TextBillboard::SetAlignment(HAlignment horizontal,
    VAlignment vertical) {
  h_align_ = horizontal;
  v_align_ = vertical;
  Recompute();
}

void TextBillboard::Recompute() {
  font_resource_ = resources_->Font(qfont_);

  GeometryData gdata;
  gdata.gl_mode = GL_TRIANGLES;

  const QVector3D normal(0, 0, 1);

  const float x_dir = 1;
  const float y_dir = -1;
  const float y_step = y_dir * line_height_;

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
    const float y0 = cursor_y + y_dir * cdata.y0 * line_height_;
    const float y1 = cursor_y + y_dir * cdata.y1 * line_height_;

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
  text_material_->SetTexture("texture", font_resource_->Texture());

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
}

}  // namespace sceneview
