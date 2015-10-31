// Copyright [2015] Albert Huang

#include "sceneview/draw_node.hpp"

#include <vector>

namespace sv {

DrawNode::DrawNode(const QString& name) :
  SceneNode(name),
  drawables_(),
  bounding_box_(),
  bounding_box_dirty_(true) {}

DrawNode::~DrawNode() {
  for (Drawable::Ptr& drawable : drawables_) {
    drawable->RemoveListener(this);
  }
}

void DrawNode::Add(const GeometryResource::Ptr& geometry,
    const MaterialResource::Ptr& material) {
  Add(Drawable::Create(geometry, material));
}

void DrawNode::Add(const Drawable::Ptr& drawable) {
  drawables_.push_back(drawable);
  drawable->AddListener(this);
  BoundingBoxChanged();
}

const std::vector<Drawable::Ptr>&
DrawNode::Drawables() const {
  return drawables_;
}

const AxisAlignedBox& DrawNode::WorldBoundingBox() {
  if (bounding_box_dirty_) {
    bounding_box_ = AxisAlignedBox();
    for (auto& item : drawables_) {
      bounding_box_.IncludeBox(item->BoundingBox());
    }
    bounding_box_ = bounding_box_.Transformed(WorldTransform());
    bounding_box_dirty_ = false;
  }
  return bounding_box_;
}

void DrawNode::BoundingBoxChanged() {
  SceneNode::BoundingBoxChanged();
  bounding_box_dirty_ = true;
}

}  // namespace sv
