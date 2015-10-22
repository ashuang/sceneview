// Copyright [2015] Albert Huang

#include "sceneview/scene_node.hpp"

namespace sv {

SceneNode::SceneNode(const QString& node_name) :
  node_name_(node_name),
  translation_(),
  rotation_(),
  scale_(1, 1, 1),
  parent_node_(nullptr),
  visible_(true) {}

const QMatrix4x4& SceneNode::GetTransform() {
  transform_.setToIdentity();
  transform_.translate(translation_);
  transform_.rotate(rotation_);
  transform_.scale(scale_);
  return transform_;
}

void SceneNode::SetTranslation(const QVector3D& vec) {
  translation_ = vec;
}

void SceneNode::SetRotation(const QQuaternion& quat) {
  rotation_ = quat;
}

void SceneNode::SetScale(const QVector3D& vec) {
  scale_ = vec;
}

void SceneNode::SetVisible(bool visible) {
  visible_ = visible;
}

void SceneNode::SetParentNode(GroupNode* parent) {
  parent_node_ = parent;
}

}  // namespace sv
