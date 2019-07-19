#include "draw_group.hpp"

namespace sv {

struct DrawGroup::Priv {
  QString name;

  int order;

  NodeOrdering node_ordering = NodeOrdering::kBackToFront;

  bool frustum_culling = true;

  CameraNode* camera = nullptr;

  std::unordered_set<DrawNode*> nodes;
};

DrawGroup::~DrawGroup() {
  delete p_;
}

const QString& DrawGroup::Name() const { return p_->name; }

int DrawGroup::Order() const { return p_->order; }

const std::unordered_set<DrawNode*>& DrawGroup::DrawNodes() const { return p_->nodes; }

/**
 * Sets the sorting method for drawing nodes in this draw group.
 */
void DrawGroup::SetNodeOrdering(NodeOrdering ordering) { p_->node_ordering = ordering; }

NodeOrdering DrawGroup::GetNodeOrdering() const { return p_->node_ordering; }

void DrawGroup::SetFrustumCulling(bool value) { p_->frustum_culling = value; }

bool DrawGroup::GetFrustumCulling() const { return p_->frustum_culling; }

void DrawGroup::SetCamera(CameraNode* camera) { p_->camera = camera; }

CameraNode* DrawGroup::GetCamera() { return p_->camera; }

DrawGroup::DrawGroup(const QString& name, int order) : p_(new Priv()) {
  p_->name = name;
  p_->order = order;
}

void DrawGroup::AddNode(DrawNode* node) {
  p_->nodes.insert(node);
}

void DrawGroup::RemoveNode(DrawNode* node) {
  p_->nodes.erase(node);
}

}  // namespace sv
