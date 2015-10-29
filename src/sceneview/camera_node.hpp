// Copyright [2015] Albert Huang

#ifndef BOT3_CAMERA_NODE_HPP__
#define BOT3_CAMERA_NODE_HPP__

#include <QSize>
#include <QVector3D>

#include <sceneview/scene_node.hpp>

namespace sv {

/**
 * Camera.
 *
 * Computes transforms between:
 * - world space (the 3D Cartesian coordinate frame the camera moves around in)
 * - eye space (the 3D Cartesian coordinate frame with the camera at the origin)
 * - projection space (3D OpenGL clip coordinates)
 * - screen space (2D window corresponding to the Viewport widget)
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/camera_node.hpp
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

    /**
     * Sets the size of the viewport.
     *
     * You generally don't need to call this method. When a camera is attached
     * to the viewport, the Viewport class automatically calls this method.
     */
    void SetViewportSize(int width, int height);

    /**
     * Retrieve the viewport size.
     */
    QSize GetViewportSize() const;

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

    /**
     * Retrieve the current projection type.
     */
    ProjectionType GetProjectionType() const { return proj_type_; }

    /**
     * Rotate and translate the camera to look at the specified point.
     */
    void LookAt(const QVector3D& eye, const QVector3D& look_at,
        const QVector3D& up);

    /**
     * Retrieve the vertical field of view, in degrees.
     */
    double GetVFovDeg() const { return vfov_deg_; }

    /**
     * Retrieve the near clipping plane.
     */
    double GetZNear() const { return z_near_; }

    /**
     * Retrieve the far clipping plane.
     */
    double GetZFar() const { return z_far_; }

    /**
     * Retrieve the direction the camera is facing.
     */
    QVector3D GetLookDir() const;

    /**
     * Retrieve the look at point.
     */
    QVector3D GetLookAt() const;

    /**
     * Retrieve the camera's up vector.
     */
    QVector3D GetUpDir() const;

    /**
     * Computes a world-space direction corresponding to the specified
     * screen-space pixel.
     *
     * The resulting vector is guaranteed to be unit length.
     */
    QVector3D Unproject(double x, double y);

    /**
     * Retrieve the camera projection matrix.
     */
    QMatrix4x4 GetProjectionMatrix();

    /**
     * Retrieve the camera view matrix.
     *
     * The view matrix transforms from world Cartesian coordinates to
     * camera-centric Cartesian coordinates.
     */
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

}  // namespace sv

#endif  // SCENEVIEW_VISCAMERA_HPP__
