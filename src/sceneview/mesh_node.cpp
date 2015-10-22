// Copyright [2015] Albert Huang

#include "sceneview/mesh_node.hpp"

#include <vector>

namespace sv {

MeshNode::MeshNode(const QString& name) :
  SceneNode(name),
  drawables_() {
}

void MeshNode::Add(const GeometryResource::Ptr& geometry,
    const MaterialResource::Ptr& material) {
  drawables_.emplace_back(Drawable::Create(geometry, material));
}

void MeshNode::Add(const Drawable::Ptr& drawable) {
  drawables_.push_back(drawable);
}

const std::vector<Drawable::Ptr>&
MeshNode::Drawables() const {
  return drawables_;
}

AxisAlignedBox MeshNode::BoundingBox(const QMatrix4x4& lhs_transform) {
  return GeometryBoundingBox().Transformed(lhs_transform * GetTransform());
}

AxisAlignedBox MeshNode::GeometryBoundingBox() {
  AxisAlignedBox result;
  for (auto& item : drawables_) {
    const GeometryResource::Ptr& geometry = item->Geometry();
    result.IncludeBox(geometry->BoundingBox());
  }
  return result;
}


}  // namespace sv
