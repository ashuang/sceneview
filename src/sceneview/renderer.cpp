// Copyright [2015] Albert Huang

#include "sceneview/renderer.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/viewport.hpp"

#include <string>

namespace sv {

Renderer::Renderer(const QString& name, QObject* parent) :
  QObject(parent),
  name_(name),
  enabled_(true) {
}

Scene::Ptr Renderer::GetScene() {
  return viewport_->GetScene();
}

ResourceManager::Ptr Renderer::GetResources() {
  return viewport_->GetResources();
}

GroupNode* Renderer::GetBaseNode() {
  return base_node_;
}

void Renderer::SetViewport(Viewport* viewport) {
  viewport_ = viewport;
}

void Renderer::SetBaseNode(GroupNode* node) {
  base_node_ = node;
}

void Renderer::SetEnabled(bool enabled) {
  if (enabled_ == enabled) {
    return;
  }

  enabled_ = enabled;
  base_node_->SetVisible(enabled_);
  OnEnableChanged(enabled_);
}

}  // namespace sv
