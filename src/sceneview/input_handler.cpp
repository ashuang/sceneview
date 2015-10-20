// Copyright [2015] Albert Huang

#include "sceneview/input_handler.hpp"

namespace sceneview {

void InputHandler::MousePressEvent(QMouseEvent* event) {
  event->ignore();
}

void InputHandler::MouseMoveEvent(QMouseEvent* event) {
  event->ignore();
}

void InputHandler::MouseReleaseEvent(QMouseEvent* event) {
  event->ignore();
}

void InputHandler::WheelEvent(QWheelEvent* event) {
  event->ignore();
}

void InputHandler::KeyPressEvent(QKeyEvent* event) {
  event->ignore();
}

void InputHandler::KeyReleaseEvent(QKeyEvent* event) {
  event->ignore();
}

}  // namespace sceneview
