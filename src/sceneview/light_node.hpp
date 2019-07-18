// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_LIGHT_NODE_HPP__
#define SCENEVIEW_LIGHT_NODE_HPP__

#include <memory>
#include <vector>

#include <sceneview/scene_node.hpp>

namespace sv {

/**
 * Specifies a light type.
 *
 * @ingroup sv_scenegraph
 */
enum class LightType { kPoint, kDirectional, kSpot };

/**
 * A light in a scene graph used by some shaders to calculate lighting effects.
 *
 * This class cannot be directly instantiated. Instead, use Scene.
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/light_node.hpp
 */
class LightNode : public SceneNode {
 public:
  virtual ~LightNode();

  SceneNodeType NodeType() const override { return SceneNodeType::kLightNode; }

  /**
   * Sets the light type.
   */
  void SetLightType(LightType light_type);

  LightType GetLightType() const;

  void SetDirection(const QVector3D& dir);

  /**
   * Sets the light direction.
   *
   * Only useful for directional and spot lights.
   */
  const QVector3D& Direction() const;

  float Ambient() const;

  /**
   * Sets the ambient coefficient for this light.
   */
  void SetAmbient(const float ambient);

  float Specular() const;

  /**
   * Sets the specular coefficient for this light.
   */
  void SetSpecular(const float specular);

  const QVector3D& Color() const;

  void SetColor(const QVector3D& color);

  /**
   * Sets the attenuation factor.
   *
   * Only useful for point and spot lights.
   */
  void SetAttenuation(const float val);

  float Attenuation() const;

  /**
   * Sets the light cone angle (in degrees).
   *
   * Only useful for spot lights.
   */
  void SetConeAngle(float cone_angle_deg);

  /**
   * Retrieve the cone angle, in degrees.
   */
  float ConeAngle() const;

  /**
   * Disabled for this class.
   */
  void SetScale(const QVector3D& vec) override;

  const AxisAlignedBox& WorldBoundingBox() override;

 private:
  friend class Scene;

  explicit LightNode(const QString& name);

  static const AxisAlignedBox kBoundingBox;

  struct Priv;

  Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_LIGHT_NODE_HPP__
