// Copyright [2015] Albert Huang

#include "sceneview/draw_node.hpp"

#include <vector>

namespace sv {

struct DrawNode::Priv {
  std::vector<Drawable::Ptr> drawables;

  AxisAlignedBox bounding_box;
  bool bounding_box_dirty;

  DrawGroup* draw_group = nullptr;
};

DrawNode::DrawNode(const QString& name) : SceneNode(name), p_(new Priv) {
  p_->bounding_box_dirty = true;
}

DrawNode::~DrawNode() {
  for (Drawable::Ptr& drawable : p_->drawables) {
    drawable->RemoveListener(this);
  }
  delete p_;
}

void DrawNode::Add(const GeometryResource::Ptr& geometry,
                   const MaterialResource::Ptr& material) {
  Add(Drawable::Create(geometry, material));
}

void DrawNode::Add(const Drawable::Ptr& drawable) {
  p_->drawables.push_back(drawable);
  drawable->AddListener(this);
  BoundingBoxChanged();
}

const std::vector<Drawable::Ptr>& DrawNode::Drawables() const {
  return p_->drawables;
}

const AxisAlignedBox& DrawNode::WorldBoundingBox() {
  if (p_->bounding_box_dirty) {
    p_->bounding_box = AxisAlignedBox();
    for (auto& item : p_->drawables) {
      p_->bounding_box.IncludeBox(item->BoundingBox());
    }
    p_->bounding_box = p_->bounding_box.Transformed(WorldTransform());
    p_->bounding_box_dirty = false;
  }
  return p_->bounding_box;
}

void DrawNode::BoundingBoxChanged() {
  SceneNode::BoundingBoxChanged();
  p_->bounding_box_dirty = true;
}

DrawGroup* DrawNode::GetDrawGroup() { return p_->draw_group; }

void DrawNode::SetDrawGroup(DrawGroup* draw_group) {
  p_->draw_group = draw_group;
}

}  // namespace sv
