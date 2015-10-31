// Copyright [2015] Albert Huang

#include "sceneview/scene_node.hpp"
#include "sceneview/group_node.hpp"

namespace sv {

SceneNode::SceneNode(const QString& node_name) :
  node_name_(node_name),
  translation_(),
  rotation_(),
  scale_(1, 1, 1),
  to_world_(),
  to_world_dirty_(true),
  parent_node_(nullptr),
  visible_(true),
  selection_mask_(0) {}

const QMatrix4x4& SceneNode::WorldTransform() {
  if (to_world_dirty_) {
    if (parent_node_) {
      to_world_ = parent_node_->WorldTransform();
    } else {
      to_world_.setToIdentity();
    }
    to_world_.translate(translation_);
    to_world_.rotate(rotation_);
    to_world_.scale(scale_);
    to_world_dirty_ = false;
  }
  return to_world_;
}

void SceneNode::SetTranslation(const QVector3D& vec) {
  translation_ = vec;
  TransformChanged();
}

void SceneNode::SetRotation(const QQuaternion& quat) {
  rotation_ = quat;
  TransformChanged();
}

void SceneNode::SetScale(const QVector3D& vec) {
  scale_ = vec;
  TransformChanged();
}

void SceneNode::SetVisible(bool visible) {
  visible_ = visible;
}

void SceneNode::SetParentNode(GroupNode* parent) {
  parent_node_ = parent;
}

void SceneNode::TransformChanged() {
  to_world_dirty_ = true;
  BoundingBoxChanged();
}

void SceneNode::BoundingBoxChanged() {
  if (parent_node_) {
    parent_node_->BoundingBoxChanged();
  }
}

}  // namespace sv
