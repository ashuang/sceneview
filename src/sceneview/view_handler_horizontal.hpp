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
 * ## Customizability
 *
 * - Azimuth and elevation control (right mouse button dragging) can be
 *   disabled programmatically via SetAllowAzimuthElevationControl()
 *
 * - The look at point can be shown or hidden during interactions with
 *   SetShowLookAtPoint()
 *
 * @ingroup sv_gui
 * @headerfile sceneview/view_handler_horizontal.hpp
 */
class ViewHandlerHorizontal : public QObject, public InputHandler {
  Q_OBJECT

 public:
  /**
   * Constructor.
   *
   * @param viewport the viewport that the view handler is associated with.
   * @param zenith_dir the zenith direction.
   * @param parent the QObject parent.
   */
  ViewHandlerHorizontal(Viewport* viewport, const QVector3D& zenith_dir,
                        QObject* parent = nullptr);

  virtual ~ViewHandlerHorizontal();

  /**
   * Retrieve the name of this view handler.
   */
  QString Name() const { return "Camera controls"; }

  /**
   * Sets the zenith direction.
   */
  void SetZenithDir(const QVector3D& dir);

  /**
   * Retrieve the zenith direction.
   */
  const QVector3D& ZenithDir() const;

  /**
   * Handles a mouse press event. This is generally invoked by Viewport.
   */
  void MousePressEvent(QMouseEvent* event) override;

  /**
   * Handles a mouse movement event. This is generally invoked by Viewport.
   */
  void MouseMoveEvent(QMouseEvent* event) override;

  /**
   * Handles a mouse wheel event. This is generally invoked by Viewport.
   */
  void WheelEvent(QWheelEvent* event) override;

  /**
   * Handles a key press event. This is generally invoked by Viewport.
   */
  void KeyPressEvent(QKeyEvent* event) override;

  /**
   * Retrieve the widget for the input handler.
   * This is typically invoked by InputHandlerWidgetStack.
   */
  QWidget* GetWidget() override;

  /**
   * Set whether or not the look at point is shown in the scene during a
   * mouse or key event.
   *
   * If enabled, the look at point is shown as a visual reference for the
   * user when interacting with the scene.
   */
  void SetShowLookAtPoint(bool val);

  /**
   * Enables / disables azimuth and elevation control.
   *
   * If disabled, then azimuth and elevation control is not allowed.
   */
  void SetAllowAzimuthElevationControl(bool val);

 private slots:
  void OnProjectionSelectionChanged();

 private:
  void MakeShape();

  void UpdateShapeTransform();

  void UpdateNearFarPlanes();

  double PivotDistance() const;

  struct Priv;
  Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_VIEW_HANDLER_HORIZONTAL__
