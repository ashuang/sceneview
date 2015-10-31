#include "drawable.hpp"

#include "sceneview/draw_node.hpp"

namespace sv {

Drawable::Drawable(const GeometryResource::Ptr& geometry,
    const MaterialResource::Ptr& material) :
  geometry_(geometry), material_(material) {
  if (geometry_) {
    geometry_->AddListener(this);
  }
}

Drawable::~Drawable() {
  if (geometry_) {
    geometry_->RemoveListener(this);
  }
}

void Drawable::SetMaterial(const MaterialResource::Ptr& material) {
  material_ = material;
}

void Drawable::BoundingBoxChanged() {
  for (DrawNode* listener : listeners_) {
    listener->BoundingBoxChanged();
  }
}

void Drawable::AddListener(DrawNode* listener) {
  listeners_.push_back(listener);
}

void Drawable::RemoveListener(DrawNode* listener) {
  auto iter = std::find(listeners_.begin(), listeners_.end(), listener);
  if (iter != listeners_.end()) {
    listeners_.erase(iter);
  }
}

}  // namespace sv
