#include <sceneview/mesh_node.hpp>

namespace sceneview {

MeshNode::MeshNode(const QString& name) :
  SceneNode(name),
  components_() {
}

void MeshNode::Add(const GeometryResource::Ptr& geometry,
    const MaterialResource::Ptr material) {
  components_.emplace_back(geometry, material);
}

void MeshNode::Add(GeometryMaterialPair component) {
  components_.push_back(component);
}

const std::vector<GeometryMaterialPair>&
MeshNode::Components() const {
  return components_;
}

AxisAlignedBox MeshNode::BoundingBox(const QMatrix4x4& lhs_transform) {
  return GeometryBoundingBox().Transformed(lhs_transform * GetTransform());
}

AxisAlignedBox MeshNode::GeometryBoundingBox() {
  AxisAlignedBox result;
  for (auto& item : components_) {
    const GeometryResource::Ptr& geometry = item.first;
    result.IncludeBox(geometry->BoundingBox());
  }
  return result;
}


}  // namespace sceneview
