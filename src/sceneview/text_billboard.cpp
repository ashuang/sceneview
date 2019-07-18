// Copyright [2015] Albert Huang

#include "sceneview/text_billboard.hpp"

#include <QColor>
#include <algorithm>

#include "sceneview/camera_node.hpp"
#include "sceneview/draw_context.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/stock_resources.hpp"
#include "sceneview/viewport.hpp"

namespace sv {

struct TextBillboard::Priv {
  Viewport* viewport;
  ResourceManager::Ptr resources;
  Scene::Ptr scene;

  GroupNode* parent;
  GroupNode* node;

  FontResource::Ptr font_resource;
  MaterialResource::Ptr bg_material;
  GeometryResource::Ptr rect_geom;
  MaterialResource::Ptr text_material;
  GeometryResource::Ptr text_geom;
  DrawNode* draw_node;

  VAlignment v_align;
  HAlignment h_align;
  float line_height;
  YDirection y_dir = kNegative;

  float margin_top;
  float margin_left;
  float margin_bottom;
  float margin_right;

  QFont qfont;
  std::string text;

  TextBillboardDrawable* bg_drawable;
  TextBillboardDrawable* text_drawable;
};

/**
 * Use a custom drawable for the text billboard so that we can report a
 * different bounding box than would be expected by the geometry.
 */
class TextBillboardDrawable : public Drawable {
 public:
  TextBillboardDrawable(const GeometryResource::Ptr& geometry,
                        const MaterialResource::Ptr& material)
      : Drawable(geometry, material) {}

  // The bounding box depends on the camera, so recompute it every time
  // it's requested.
  const AxisAlignedBox& BoundingBox() override { return aabb_; }

  void SetBoundingBox(const AxisAlignedBox& box) { aabb_ = box; }

 private:
  AxisAlignedBox aabb_;
};

TextBillboard::Ptr TextBillboard::Create(Viewport* viewport,
                                         GroupNode* parent) {
  return Ptr(new TextBillboard(viewport, parent));
}

TextBillboard::TextBillboard(Viewport* viewport, GroupNode* parent)
    : p_(new Priv) {
  p_->viewport = viewport;
  p_->resources = p_->viewport->GetResources();
  p_->scene = p_->viewport->GetScene();
  p_->parent = parent;
  p_->v_align = kVCenter;
  p_->h_align = kHCenter;
  p_->line_height = 0.1;
  p_->margin_top = 0.2;
  p_->margin_left = 0.2;
  p_->margin_bottom = 0.2;
  p_->margin_right = 0.2;
  p_->qfont = QFont("Helvetica");

  // Implementation details
  // The text display is rendered as two layered geometries:
  // 1. Background layer (a single rectangle)
  // 2. Text layer (one textured quad per character)

  StockResources stock(p_->resources);

  // Create the background geometry and material
  ShaderResource::Ptr billboard_uniform_color =
      stock.Shader(StockResources::kBillboardUniformColor);
  p_->bg_material = p_->resources->MakeMaterial(billboard_uniform_color);
  p_->bg_material->SetParam("color", 0.0, 0.0, 0.0, 0.0);
  p_->bg_material->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  p_->bg_material->SetTwoSided(true);

  // Create geometry for the background and the cap
  p_->rect_geom = p_->resources->MakeGeometry();

  // Create the text geometry and material
  p_->text_material = p_->resources->MakeMaterial(
      stock.Shader(StockResources::kBillboardTextured));
  p_->text_material->SetTwoSided(true);
  p_->text_material->SetDepthFunc(GL_LEQUAL);
  p_->text_material->SetParam("text_color", 1.0, 1.0, 1.0, 1.0);
  p_->text_material->SetBlend(true);
  p_->text_material->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  p_->text_geom = p_->resources->MakeGeometry();

  p_->node = p_->scene->MakeGroup(p_->parent);
  p_->draw_node = p_->scene->MakeDrawNode(p_->node);

  // Use a custom Drawable to correctly compute the node bounding box.
  p_->bg_drawable = new TextBillboardDrawable(p_->rect_geom, p_->bg_material);
  p_->text_drawable =
      new TextBillboardDrawable(p_->text_geom, p_->text_material);
  p_->draw_node->Add(Drawable::Ptr(p_->bg_drawable));
  p_->draw_node->Add(Drawable::Ptr(p_->text_drawable));
}

TextBillboard::~TextBillboard() {
  p_->scene->DestroyNode(p_->node);
  p_->font_resource.reset();
  p_->text_geom.reset();
  p_->text_material.reset();
  delete p_;
}

void TextBillboard::SetText(const QString& text) {
  p_->text = text.toStdString();
  Recompute();
}

void TextBillboard::SetLineHeight(float height) {
  p_->line_height = height;
  Recompute();
}

void TextBillboard::SetFont(const QFont& font) {
  p_->qfont = font;
  Recompute();
}

void TextBillboard::SetTextColor(const QColor& color) {
  p_->text_material->SetParam("text_color", color.redF(), color.greenF(),
                              color.blueF(), color.alphaF());
}

void TextBillboard::SetBackgroundColor(const QColor& color) {
  p_->bg_material->SetParam("color", color.redF(), color.greenF(),
                            color.blueF(), color.alphaF());
  p_->bg_material->SetBlend(color.alphaF() < 1);
}

void TextBillboard::SetAlignment(HAlignment horizontal, VAlignment vertical) {
  p_->h_align = horizontal;
  p_->v_align = vertical;
  Recompute();
}

void TextBillboard::SetYDirection(YDirection direction) {
  if (direction != kPositive && direction != kNegative) {
    throw std::invalid_argument("Invalid direction");
  }
  p_->y_dir = direction;
}

GroupNode* TextBillboard::Node() { return p_->node; }

void TextBillboard::Recompute() {
  p_->font_resource = p_->resources->Font(p_->qfont);

  GeometryData gdata;
  gdata.gl_mode = GL_TRIANGLES;

  const QVector3D normal(0, 0, 1);

  const float x_dir = 1;
  const float y_step = p_->y_dir * p_->line_height;

  float cursor_x = 0;
  float cursor_y = 0;
  float min_x = 0;
  float max_x = 0;
  float min_y = 0;
  float max_y = 0;

  // Place each character in the text string and compute both its vertex
  // positions and texture coordinates.
  int base_vertex = 0;
  for (unsigned ch_index = 0; ch_index < p_->text.size(); ++ch_index) {
    const char ch = p_->text[ch_index];
    if (ch == '\n') {
      cursor_x = 0;
      cursor_y += y_step;
      continue;
    }

    const FontResource::CharData& cdata = p_->font_resource->GetCharData(ch);
    const float x0 = cursor_x + x_dir * cdata.x0 * p_->line_height;
    const float x1 = cursor_x + x_dir * cdata.x1 * p_->line_height;
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

    cursor_x += cdata.width_to_height * p_->line_height;
  }

  // Align vertices relative to anchor point.
  float x_offset = 0;
  float y_offset = 0;
  switch (p_->h_align) {
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

  if (p_->y_dir == kPositive) {
    switch (p_->v_align) {
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
    switch (p_->v_align) {
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
  const float margin_left_actual = p_->margin_left * p_->line_height;
  const float margin_right_actual = p_->margin_right * p_->line_height;
  const float margin_top_actual = p_->margin_top * p_->line_height;
  const float margin_bottom_actual = p_->margin_bottom * p_->line_height;

  const QVector3D offset(x_offset + margin_left_actual,
                         y_offset + margin_bottom_actual, 0);

  for (auto& vertex : gdata.vertices) {
    vertex += offset;
  }

  // Done calculating the text geometry. Load it into the geometry resource.
  p_->text_geom->Load(gdata);

  // Make sure the material is configured to use the active font.
  p_->text_material->AddTexture("texture", p_->font_resource->Texture());

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
  p_->rect_geom->Load(rdata);

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

  p_->bg_drawable->SetBoundingBox(box);
  p_->text_drawable->SetBoundingBox(box);
}

}  // namespace sv
