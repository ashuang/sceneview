// Copyright [2015] Albert Huang

#include "sceneview/scene_node.hpp"
#include "sceneview/group_node.hpp"

namespace sv {

struct SceneNode::Priv {
  QString node_name;

  QVector3D translation;
  QQuaternion rotation;
  QVector3D scale{1, 1, 1};

  QMatrix4x4 to_world;
  bool to_world_dirty = true;

  GroupNode* parent_node = nullptr;

  bool visible = true;
  int64_t selection_mask = 0;

  int draw_order = 0;
};

SceneNode::SceneNode(const QString& node_name) : p_(new Priv()) {
  p_->node_name = node_name;
}

SceneNode::~SceneNode()
{
  delete p_;
}

const QString SceneNode::Name() const { return p_->node_name; }

/**
 * Retrieve the translation component of the node to parent transform.
 */
const QVector3D& SceneNode::Translation() const { return p_->translation; }

/**
 * Retrieve the rotation component of the node to parent transform.
 */
const QQuaternion& SceneNode::Rotation() const { return p_->rotation; }

/**
 * Retrieve the scale component of the node to parent transform.
 */
const QVector3D& SceneNode::Scale() const { return p_->scale; }

const QMatrix4x4& SceneNode::WorldTransform() {
  if (p_->to_world_dirty) {
    if (p_->parent_node) {
      p_->to_world = p_->parent_node->WorldTransform();
    } else {
      p_->to_world.setToIdentity();
    }
    p_->to_world.translate(p_->translation);
    p_->to_world.rotate(p_->rotation);
    p_->to_world.scale(p_->scale);
    p_->to_world_dirty = false;
  }
  return p_->to_world;
}

bool SceneNode::Visible() const { return p_->visible; }

void SceneNode::SetTranslation(const QVector3D& vec) {
  p_->translation = vec;
  TransformChanged();
}

void SceneNode::SetRotation(const QQuaternion& quat) {
  p_->rotation = quat;
  TransformChanged();
}

void SceneNode::SetScale(const QVector3D& vec) {
  p_->scale = vec;
  TransformChanged();
}

void SceneNode::SetVisible(bool visible) {
  p_->visible = visible;
}

GroupNode* SceneNode::ParentNode() { return p_->parent_node; }

void SceneNode::SetParentNode(GroupNode* parent) {
  p_->parent_node = parent;
}

void SceneNode::SetSelectionMask(int64_t mask) { p_->selection_mask = mask; }

/**
 * Retrieve the selection mask for this node.
 */
int64_t SceneNode::GetSelectionMask() const { return p_->selection_mask; }

void SceneNode::SetDrawOrder(int order) { p_->draw_order = order; }

int SceneNode::DrawOrder() const { return p_->draw_order; }

void SceneNode::TransformChanged() {
  p_->to_world_dirty = true;
  BoundingBoxChanged();
}

void SceneNode::BoundingBoxChanged() {
  if (p_->parent_node) {
    p_->parent_node->BoundingBoxChanged();
  }
}

}  // namespace sv
