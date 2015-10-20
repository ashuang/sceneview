// Copyright [2015] Albert Huang

#include "sceneview/light_node.hpp"

namespace sv {

LightNode::LightNode(const QString& name) :
  SceneNode(name),
  light_type_(LightType::kPoint),
  direction_(0.0, 0.0, 1.0),
  color_(1.0, 1.0, 1.0),
  ambient_(0.1),
  attenuation_(0.0f),
  cone_angle_deg_(180.0f) {}

void LightNode::SetScale(const QVector3D& vec) {
  if (vec != QVector3D(1, 1, 1)) {
    throw std::invalid_argument("not allowed");
  }
  SceneNode::SetScale(vec);
}

}  // namespace sv
