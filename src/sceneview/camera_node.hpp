// Copyright [2015] Albert Huang

#ifndef BOT3_CAMERA_NODE_HPP__
#define BOT3_CAMERA_NODE_HPP__

#include <sceneview/scene_node.hpp>

namespace sceneview {

/**
 * Camera.
 *
 * Computes transforms between:
 * - world space (the 3D Cartesian coordinate frame the camera moves around in)
 * - eye space (the 3D Cartesian coordinate frame with the camera at the origin)
 * - projection space (3D OpenGL clip coordinates)
 * - screen space (2D window corresponding to the Viewport widget)
 */
class CameraNode : public SceneNode {
  public:
    enum ProjectionType {
      kOrthographic,
      kPerspective
    };

    SceneNodeType NodeType() const override {
      return SceneNodeType::kCameraNode; }

    /**
     * Copies parameters in from the specified camera.
     *
     * Attributes copied in are:
     * - extrinsics (eye, look, up vectors)
     * - projection parameters (vfov, clipping planes, etc.)
     * - viewport size
     */
    void CopyFrom(const CameraNode& other);

    void SetViewportSize(int width, int height);

    /**
     * Sets the projection parameters.
     * @param type either kOrthographic or kPerspective
     * @param vfov_deg vertical field of view, in degrees. For orthographic
     *                 projection, this is calculated at the look_at point.
     * @param z_near near clipping plane
     * @param z_var far clipping plane
     */
    void SetProjectionParams(ProjectionType type,
        double vfov_deg, double z_near, double z_far);

    ProjectionType GetProjectionType() const { return proj_type_; }

    void LookAt(const QVector3D& eye, const QVector3D& look_at,
        const QVector3D& up);

    /**
     * Retrieve the vertical field of view, in degrees.
     */
    double GetVFovDeg() const { return vfov_deg_; }

    double GetZNear() const { return z_near_; }

    double GetZFar() const { return z_far_; }

    /**
     * @return the direction the camera is facing.
     */
    QVector3D GetLookDir() const;

    /**
     * @return the look at point.
     */
    QVector3D GetLookAt() const;

    /**
     * @return the camera's up vector.
     */
    QVector3D GetUpDir() const;

    /**
     * Computes a world-space direction corresponding to the specified
     * screen-space pixel.
     */
    QVector3D Unproject(double x, double y);

    QMatrix4x4 GetProjectionMatrix();

    QMatrix4x4 GetViewMatrix();

    /**
     * Gets the combined projection and view matrix.
     *
     * Equivalent to:
     * @code
     * GetProjectionMatrix() * GetViewMatrix();
     * @endcode
     */
    QMatrix4x4 GetViewProjectionMatrix();

  private:
    friend class Scene;

    explicit CameraNode(const QString& name);

    void ComputeProjectionMatrix();

    void SetTranslation(const QVector3D& vec) override;

    void SetRotation(const QQuaternion& quat) override;

    QVector3D look_;
    QVector3D up_;
    QVector3D look_at_;

    int viewport_width_;
    int viewport_height_;

    ProjectionType proj_type_;

    double vfov_deg_;

    double z_near_;
    double z_far_;

    QMatrix4x4 projection_matrix_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_VISCAMERA_HPP__
