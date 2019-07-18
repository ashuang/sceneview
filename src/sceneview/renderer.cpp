// Copyright [2015] Albert Huang

#include "sceneview/renderer.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/viewport.hpp"

#include <string>

namespace sv {

struct Renderer::Priv {
  QString name;

  Viewport* viewport;

  GroupNode* base_node;

  bool enabled;
};

Renderer::Renderer(const QString& name, QObject* parent)
    : QObject(parent), p_(new Priv) {
  p_->name = name;
  p_->enabled = true;
}

Renderer::~Renderer() { delete p_; }

const QString& Renderer::Name() const { return p_->name; }

Viewport* Renderer::GetViewport() { return p_->viewport; }

Scene::Ptr Renderer::GetScene() { return p_->viewport->GetScene(); }

ResourceManager::Ptr Renderer::GetResources() {
  return p_->viewport->GetResources();
}

GroupNode* Renderer::GetBaseNode() { return p_->base_node; }

void Renderer::SetViewport(Viewport* viewport) { p_->viewport = viewport; }

void Renderer::SetBaseNode(GroupNode* node) { p_->base_node = node; }

void Renderer::SetEnabled(bool enabled) {
  if (p_->enabled == enabled) {
    return;
  }

  p_->enabled = enabled;
  p_->base_node->SetVisible(p_->enabled);
  EnableChanged(enabled);
  OnEnableChanged(p_->enabled);
}

bool Renderer::Enabled() const { return p_->enabled; }

}  // namespace sv
