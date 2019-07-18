#include "drawable.hpp"

#include "sceneview/draw_node.hpp"

namespace sv {

struct Drawable::Priv {
  std::vector<DrawNode*> listeners;

  GeometryResource::Ptr geometry;
  MaterialResource::Ptr material;
};

Drawable::Drawable(const GeometryResource::Ptr& geometry,
    const MaterialResource::Ptr& material) :
  p_(new Priv) {
  p_->geometry = geometry;
  p_->material = material;
  if (p_->geometry) {
    p_->geometry->AddListener(this);
  }
}

Drawable::~Drawable() {
  if (p_->geometry) {
    p_->geometry->RemoveListener(this);
  }
  delete p_;
}

const GeometryResource::Ptr& Drawable::Geometry() { return p_->geometry; }

const MaterialResource::Ptr& Drawable::Material() { return p_->material; }

void Drawable::SetMaterial(const MaterialResource::Ptr& material) {
  p_->material = material;
}

void Drawable::BoundingBoxChanged() {
  for (DrawNode* listener : p_->listeners) {
    listener->BoundingBoxChanged();
  }
}

const AxisAlignedBox& Drawable::BoundingBox() {
  return p_->geometry->BoundingBox();
}

void Drawable::AddListener(DrawNode* listener) {
  p_->listeners.push_back(listener);
}

void Drawable::RemoveListener(DrawNode* listener) {
  auto iter = std::find(p_->listeners.begin(), p_->listeners.end(), listener);
  if (iter != p_->listeners.end()) {
    p_->listeners.erase(iter);
  }
}

}  // namespace sv
