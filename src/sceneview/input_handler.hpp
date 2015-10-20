// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_INPUT_HANDLER_HPP__
#define SCENEVIEW_INPUT_HANDLER_HPP__

#include <QMouseEvent>
#include <QKeyEvent>
#include <QVariant>

class QWidget;

namespace sceneview {

class InputHandler {
  public:
    virtual ~InputHandler() {}

    virtual QString Name() const = 0;

    /**
     * Called when the input handler is activated and given control.
     */
    virtual void Activated() {}

    /**
     * Called when the input handler is deactivated.
     */
    virtual void Deactivated() {}

    /**
     * Called when the OpenGL context is ready for use.
     */
    virtual void InitializeGL() {}

    /**
     * Called when the OpenGL context is shutting down (usually, right before
     * destruction).
     */
    virtual void ShutdownGL() {}

    virtual void MousePressEvent(QMouseEvent* event);

    virtual void MouseMoveEvent(QMouseEvent* event);

    virtual void MouseReleaseEvent(QMouseEvent* event);

    virtual void WheelEvent(QWheelEvent* event);

    virtual void KeyPressEvent(QKeyEvent* event);

    virtual void KeyReleaseEvent(QKeyEvent* event);

    virtual QWidget* GetWidget() { return nullptr; }

    virtual QVariant SaveState() { return QVariant(); }

    virtual void LoadState(const QVariant& val) {}
};

}  // namespace sceneview

#endif  // SCENEVIEW_VIEW_HANDLER_HPP__
