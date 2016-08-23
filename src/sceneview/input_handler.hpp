// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_INPUT_HANDLER_HPP__
#define SCENEVIEW_INPUT_HANDLER_HPP__

#include <QMouseEvent>
#include <QKeyEvent>
#include <QVariant>

class QWidget;

namespace sv {

/**
 * Receive and handle mouse/keyboard input events.
 *
 * This class is pure virtual and cannot be instantiated.
 *
 * Sceneview users can create subclasses of InputHandler to implement custom
 * input handlers (e.g., to interact with objects in the scene, or to provide a
 * command/control UI).
 *
 * @ingroup sv_gui
 * @headerfile sceneview/input_handler.hpp
 */
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

    /**
     * Called when the mouse is pressed in the viewport.
     */
    virtual void MousePressEvent(QMouseEvent* event);

    /**
     * Called when the mouse is moved in the viewport.
     */
    virtual void MouseMoveEvent(QMouseEvent* event);

    /**
     * Called when the mouse is released in the viewport.
     */
    virtual void MouseReleaseEvent(QMouseEvent* event);

    /**
     * Called when the mouse is double-clicked in the viewport.
     */
    virtual void MouseDoubleClickEvent(QMouseEvent* event);

    /**
     * Called when the mouse wheel is scrolled.
     */
    virtual void WheelEvent(QWheelEvent* event);

    /**
     * Called when a key is pressed in the viewport.
     */
    virtual void KeyPressEvent(QKeyEvent* event);

    /**
     * Called when a key is released in the viewport.
     */
    virtual void KeyReleaseEvent(QKeyEvent* event);

    /**
     * Called to retrieve a widget for the InputHandler to manage.
     *
     * When subclassing from InputHandler, you can create a UI for your class
     * by providing a widget here.
     */
    virtual QWidget* GetWidget() { return nullptr; }

    /**
     * Called by the viewport to save the InputHandler state. If your input
     * handler has any adjustable settings, then save them into the returned
     * QVariant.
     */
    virtual QVariant SaveState() { return QVariant(); }

    /**
     * Called by the viewport to restore the InputHandler state. If your input
     * handler has any adjustable settings, then load them from the passed in
     * QVariant here.
     */
    virtual void LoadState(const QVariant& val) {}
};

}  // namespace sv

#endif  // SCENEVIEW_VIEW_HANDLER_HPP__
