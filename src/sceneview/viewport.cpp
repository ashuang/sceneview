// Copyright [2015] Albert Huang

#include "sceneview/internal_gl.hpp"
#include "sceneview/viewport.hpp"

#include <iostream>
#include <vector>

#include <QOpenGLFunctions>
#include <QTimer>

#include "sceneview/camera_node.hpp"
#include "sceneview/draw_context.hpp"
#include "sceneview/input_handler.hpp"
#include "sceneview/light_node.hpp"
#include "sceneview/renderer.hpp"

namespace sv {

Viewport::Viewport(const ResourceManager::Ptr& resources,
    const Scene::Ptr& scene, QWidget* parent) :
  QOpenGLWidget(parent),
  resources_(resources),
  scene_(scene),
  camera_(nullptr),
  input_handler_(nullptr),
  draw_(new DrawContext(resources_, scene)),
  renderers_(),
  input_handlers_(),
  redraw_scheduled_(false),
  gl_context_(nullptr),
  background_color_(0, 0, 0, 1) {
  // Enable multisampling so that things draw a little smoother.
  QSurfaceFormat format = QSurfaceFormat::defaultFormat();
  format.setSamples(2);
  setFormat(format);
}

Viewport::~Viewport() {
  // Activate the opengl context and then shut down the renderers.
  makeCurrent();
  if (input_handler_) {
    input_handler_->Deactivated();
  }
  for (Renderer* renderer : renderers_) {
    renderer->ShutdownGL();
  }
  for (InputHandler* handler : input_handlers_) {
    handler->ShutdownGL();
  }
  renderers_.clear();
  gl_context_ = nullptr;
}

void Viewport::AddRenderer(Renderer* renderer) {
  renderers_.push_back(renderer);
  renderer->SetViewport(this);
  renderer->SetBaseNode(scene_->MakeGroup(scene_->Root(),
        "basenode_" + renderer->Name()));

  if (gl_context_) {
    makeCurrent();
    renderer->InitializeGL();
  }

  emit RendererAdded(renderer);
}

void Viewport::AddInputHandler(InputHandler* handler) {
  input_handlers_.push_back(handler);
  if (gl_context_) {
    handler->InitializeGL();
  }
  emit InputHandlerAdded(handler);

  // If it's the only input handers, activate it.
  if (input_handlers_.size() == 1) {
    ActivateInputHandler(handler);
  }
}

void Viewport::SetCamera(CameraNode* camera_node) {
  if (camera_ == camera_node) {
    return;
  }
  if (!scene_->ContainsNode(camera_node)) {
    throw std::invalid_argument("camera doesn't belong the scene");
  }
  camera_ = camera_node;
  camera_->SetViewportSize(width(), height());

  emit CameraChanged(camera_);
}

void Viewport::ScheduleRedraw() {
  if (!redraw_scheduled_) {
    QTimer::singleShot(30, this, SLOT(Render()));
    redraw_scheduled_ = true;
  }
}

void Viewport::ActivateInputHandler(InputHandler* handler) {
  if (input_handler_ == handler) {
    return;
  }
  if (input_handler_) {
    input_handler_->Deactivated();
  }
  input_handler_ = handler;
  input_handler_->Activated();
  emit InputHandlerActivated(handler);
}

void Viewport::initializeGL() {
  gl_context_ = QOpenGLContext::currentContext();

  for (Renderer* renderer : renderers_) {
    renderer->InitializeGL();
  }

  for (InputHandler* handler : input_handlers_) {
    handler->InitializeGL();
  }
}

void Viewport::resizeGL(int width, int height) {
  if (camera_) {
    camera_->SetViewportSize(width, height);
  }
}

void Viewport::paintGL() {
  redraw_scheduled_ = false;

  // Clear the drawing area
  glClearColor(background_color_.redF(),
      background_color_.greenF(),
      background_color_.blueF(),
      background_color_.alphaF());

  // Draw the scene
  draw_->Draw(camera_, &renderers_);

  // All done drawing.
  gl_context_->swapBuffers(gl_context_->surface());
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->MousePressEvent(event);
  }
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->MouseMoveEvent(event);
  }
}

void Viewport::mouseReleaseEvent(QMouseEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->MouseReleaseEvent(event);
  }
}

void Viewport::wheelEvent(QWheelEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->WheelEvent(event);
  }
}

void Viewport::keyPressEvent(QKeyEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->KeyPressEvent(event);
  }
}

void Viewport::keyReleaseEvent(QKeyEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->KeyReleaseEvent(event);
  }
}

void Viewport::Render() {
  repaint(0, 0, width(), height());
}


}  // namespace sv
