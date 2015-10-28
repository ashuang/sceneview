// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_VIEW_HANDLER_HORIZONTAL__
#define SCENEVIEW_VIEW_HANDLER_HORIZONTAL__

#include <QVector3D>

#include <sceneview/input_handler.hpp>

class QComboBox;
class QMouseEvent;
class QTimer;

namespace sv {

class CameraNode;
class DrawNode;
class Viewport;

/**
 * A view handler that has a notion of a fundamental plane (e.g., a ground
 * plane) and a zenith direction (i.e., an absolute "up" direction).
 *
 * Camera orientation is controlled as azimuth and elevation.
 *
 * ## Interactions:
 *
 * Click-dragging with the left mouse button moves the camera in a plane
 * parallel to the fundamental plane.
 *
 * Click-dragging with the middle mouse button moves the camera perpendicular
 * to the fundamental plane.
 *
 * Click-dragging with the right mouse button orbits the camera about the look
 * at point, azimuth-elevation style.
 *
 * Scrolling the wheel moves the camera closer to or farther from the look at
 * point.
 *
 * Keyboard up and down arrows move the camera perpendicular to the fundamental
 * plane (i.e., similar to click-dragging with the middle mouse button).
 *
 * @ingroup sv_gui
 * @headerfile sceneview/view_handler_horizontal.hpp
 */
class ViewHandlerHorizontal : public QObject, public InputHandler {
  Q_OBJECT

  public:
    ViewHandlerHorizontal(Viewport* viewport,
        const QVector3D& zenith_dir,
        QObject* parent = nullptr);

    virtual ~ViewHandlerHorizontal() {}

    QString Name() const { return "Camera controls"; }

    /**
     * Sets the zenith direction.
     */
    void SetZenithDir(const QVector3D& dir);

    const QVector3D& ZenithDir() const { return zenith_dir_; }

    void MousePressEvent(QMouseEvent *event) override;

    void MouseMoveEvent(QMouseEvent *event) override;

    void WheelEvent(QWheelEvent* event) override;

    void KeyPressEvent(QKeyEvent* event) override;

    QWidget* GetWidget() override;

    void SetShowLookAtPoint(bool val);

  private slots:
    void OnProjectionSelectionChanged();

  private:
    CameraNode* camera_;

    Viewport* viewport_;

    void MakeShape();

    void UpdateShapeTransform();

    void UpdateNearFarPlanes();

    double PivotDistance() const;

    double mouse_speed_;
    QVector3D zenith_dir_;

    int first_mouse_x_;
    int first_mouse_y_;

    double movement_scale_;
    QVector3D eye_start_;
    QVector3D look_start_;
    QVector3D up_start_;

    bool show_look_at_point_;
    DrawNode* look_at_shape_;
    QTimer* hide_shape_timer_;

    // Input handler widget
    QWidget* widget_;
    QComboBox* projection_combo_;
};

}  // namespace sv

#endif  // SCENEVIEW_VIEW_HANDLER_HORIZONTAL__
