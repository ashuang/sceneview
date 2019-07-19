// Copyright [2015] Albert Huang

#include "sceneview/light_node.hpp"

namespace sv {

const AxisAlignedBox LightNode::kBoundingBox;

struct LightNode::Priv {
  LightType light_type;
  QVector3D direction;
  QVector3D color;
  float ambient;
  float specular;
  float attenuation;
  float cone_angle_deg;
};

LightNode::~LightNode() { delete p_; }

void LightNode::SetLightType(LightType light_type) { p_->light_type = light_type; }

LightType LightNode::GetLightType() const { return p_->light_type; }

void LightNode::SetDirection(const QVector3D& dir) { p_->direction = dir; }

/**
 * Sets the light direction.
 *
 * Only useful for directional and spot lights.
 */
const QVector3D& LightNode::Direction() const { return p_->direction; }

float LightNode::Ambient() const { return p_->ambient; }

/**
 * Sets the ambient coefficient for this light.
 */
void LightNode::SetAmbient(const float ambient) { p_->ambient = ambient; }

float LightNode::Specular() const { return p_->specular; }

/**
 * Sets the specular coefficient for this light.
 */
void LightNode::SetSpecular(const float specular) { p_->specular = specular; }

const QVector3D& LightNode::Color() const { return p_->color; }

void LightNode::SetColor(const QVector3D& color) { p_->color = color; }

/**
 * Sets the attenuation factor.
 *
 * Only useful for point and spot lights.
 */
void LightNode::SetAttenuation(const float val) { p_->attenuation = val; }

float LightNode::Attenuation() const { return p_->attenuation; }

/**
 * Sets the light cone angle (in degrees).
 *
 * Only useful for spot lights.
 */
void LightNode::SetConeAngle(float cone_angle_deg) {
  p_->cone_angle_deg = cone_angle_deg;
}

/**
 * Retrieve the cone angle, in degrees.
 */
float LightNode::ConeAngle() const { return p_->cone_angle_deg; }

LightNode::LightNode(const QString& name) : SceneNode(name), p_(new Priv()) {
  p_->light_type = LightType::kPoint;
  p_->direction = QVector3D(0.0, 0.0, 1.0);
  p_->color = QVector3D(1.0, 1.0, 1.0);
  p_->ambient = 0.1;
  p_->specular = 0.3;
  p_->attenuation = 0.0f;
  p_->cone_angle_deg = 180.0f;
}

void LightNode::SetScale(const QVector3D& vec) {
  if (vec != QVector3D(1, 1, 1)) {
    throw std::invalid_argument("not allowed");
  }
  SceneNode::SetScale(vec);
}

const AxisAlignedBox& LightNode::WorldBoundingBox() { return kBoundingBox; }

}  // namespace sv
