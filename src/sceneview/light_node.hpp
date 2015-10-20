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
enum class LightType {
  kPoint,
  kDirectional,
  kSpot
};

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
    virtual ~LightNode() {}

    SceneNodeType NodeType() const override {
      return SceneNodeType::kLightNode; }

    /**
     * Sets the light type.
     */
    void SetLightType(LightType light_type) { light_type_ = light_type; }

    LightType GetLightType() const { return light_type_; }

    void SetDirection(const QVector3D& dir) { direction_ = dir; }

    /**
     * Sets the light direction.
     *
     * Only useful for directional and spot lights.
     */
    const QVector3D& Direction() const { return direction_; }

    float Ambient() const { return ambient_; }

    const QVector3D& Color() const { return color_; }

    void SetColor(const QVector3D& color) { color_ = color; }

    /**
     * Sets the ambient coefficient for this light.
     */
    void SetAmbient(const float ambient) { ambient_ = ambient; }

    /**
     * Sets the attenuation factor.
     *
     * Only useful for point and spot lights.
     */
    void SetAttenuation(const float val) { attenuation_ = val; }

    float Attenuation() const { return attenuation_; }

    /**
     * Sets the light cone angle (in degrees).
     *
     * Only useful for spot lights.
     */
    void SetConeAngle(float cone_angle_deg) {
      cone_angle_deg_ = cone_angle_deg;
    }

    /**
     * Retrieve the cone angle, in degrees.
     */
    float ConeAngle() const { return cone_angle_deg_; }

    /**
     * Disabled for this class.
     */
    void SetScale(const QVector3D& vec) override;

  private:
    friend class Scene;

    explicit LightNode(const QString& name);

    LightType light_type_;
    QVector3D direction_;
    QVector3D color_;
    float ambient_;
    float attenuation_;
    float cone_angle_deg_;
};

}  // namespace sv

#endif  // SCENEVIEW_LIGHT_NODE_HPP__
