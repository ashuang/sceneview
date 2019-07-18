// Copyright [2015] Albert Huang

#include "sceneview/viewport.hpp"
#include "sceneview/internal_gl.hpp"

#include <iostream>
#include <vector>

#include <QOpenGLFunctions>
#include <QPainter>
#include <QTimer>

#include "sceneview/camera_node.hpp"
#include "sceneview/draw_context.hpp"
#include "sceneview/draw_group.hpp"
#include "sceneview/input_handler.hpp"
#include "sceneview/light_node.hpp"
#include "sceneview/renderer.hpp"

namespace sv {

struct Viewport::Priv {
  ResourceManager::Ptr resources;

  Scene::Ptr scene;

  CameraNode* camera;

  InputHandler* input_handler;

  std::unique_ptr<DrawContext> draw;

  std::vector<Renderer*> renderers;

  std::vector<InputHandler*> input_handlers;

  bool redraw_scheduled;

  QOpenGLContext* gl_context;
};

Viewport::Viewport(const ResourceManager::Ptr& resources,
                   const Scene::Ptr& scene, QWidget* parent)
    : QOpenGLWidget(parent), p_(new Priv) {
  p_->resources = resources;
  p_->scene = scene;
  p_->camera = nullptr;
  p_->input_handler = nullptr;
  p_->draw.reset(new DrawContext(p_->resources, scene));
  p_->redraw_scheduled = false;
  p_->gl_context = nullptr;

  // Enable multisampling so that things draw a little smoother.
  QSurfaceFormat format = QSurfaceFormat::defaultFormat();
  format.setSamples(2);
  setFormat(format);

  setFocusPolicy(Qt::ClickFocus);

  p_->draw->SetDrawGroups({p_->scene->GetDefaultDrawGroup()});
}

Viewport::~Viewport() {
  // Activate the opengl context and then shut down the renderers.
  makeCurrent();
  if (p_->input_handler) {
    p_->input_handler->Deactivated();
  }
  emit GLShuttingDown();
  for (Renderer* renderer : p_->renderers) {
    renderer->ShutdownGL();
  }
  for (InputHandler* handler : p_->input_handlers) {
    handler->ShutdownGL();
  }
  p_->renderers.clear();
  p_->gl_context = nullptr;
}

void Viewport::AddRenderer(Renderer* renderer) {
  p_->renderers.push_back(renderer);
  renderer->SetViewport(this);
  renderer->SetBaseNode(
      p_->scene->MakeGroup(p_->scene->Root(), "basenode_" + renderer->Name()));

  if (p_->gl_context) {
    makeCurrent();
    renderer->InitializeGL();
  }

  emit RendererAdded(renderer);
}

void Viewport::AddInputHandler(InputHandler* handler) {
  p_->input_handlers.push_back(handler);
  if (p_->gl_context) {
    handler->InitializeGL();
  }
  emit InputHandlerAdded(handler);

  // If it's the only input handers, activate it.
  if (p_->input_handlers.size() == 1) {
    ActivateInputHandler(handler);
  }
}

void Viewport::SetCamera(CameraNode* camera_node) {
  if (p_->camera == camera_node) {
    return;
  }
  if (!p_->scene->ContainsNode(camera_node)) {
    throw std::invalid_argument("camera doesn't belong the scene");
  }
  p_->camera = camera_node;
  p_->camera->SetViewportSize(width(), height());
  p_->scene->GetDefaultDrawGroup()->SetCamera(p_->camera);

  emit CameraChanged(p_->camera);
}

CameraNode* Viewport::GetCamera() { return p_->camera; }

void Viewport::ScheduleRedraw() {
  if (!p_->redraw_scheduled) {
    QTimer::singleShot(30, this, SLOT(Render()));
    p_->redraw_scheduled = true;
  }
}

void Viewport::ActivateInputHandler(InputHandler* handler) {
  if (p_->input_handler == handler) {
    return;
  }
  if (p_->input_handler) {
    p_->input_handler->Deactivated();
  }
  p_->input_handler = handler;
  p_->input_handler->Activated();
  emit InputHandlerActivated(handler);
}

Scene::Ptr Viewport::GetScene() { return p_->scene; }

ResourceManager::Ptr Viewport::GetResources() { return p_->resources; }

std::vector<Renderer*> Viewport::GetRenderers() { return p_->renderers; }

std::vector<InputHandler*> Viewport::GetInputHandlers() {
  return p_->input_handlers;
}

void Viewport::SetBackgroundColor(const QColor& color) {
  p_->draw->SetClearColor(color);
}

void Viewport::SetDrawGroups(const std::vector<DrawGroup*>& groups) {
  p_->draw->SetDrawGroups(groups);
}

InputHandler* Viewport::GetActiveInputHandler() { return p_->input_handler; }

void Viewport::initializeGL() {
  p_->gl_context = QOpenGLContext::currentContext();

  for (Renderer* renderer : p_->renderers) {
    renderer->InitializeGL();
  }

  for (InputHandler* handler : p_->input_handlers) {
    handler->InitializeGL();
  }

  emit GLInitialized();
}

void Viewport::resizeGL(int width, int height) {
  if (p_->camera) {
    p_->camera->SetViewportSize(width, height);
  }
}

void Viewport::paintGL() {
  p_->redraw_scheduled = false;
  p_->draw->Draw(width(), height(), &p_->renderers);
}

void Viewport::mousePressEvent(QMouseEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->MousePressEvent(event);
  }
}

void Viewport::mouseMoveEvent(QMouseEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->MouseMoveEvent(event);
  }
}

void Viewport::mouseReleaseEvent(QMouseEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->MouseReleaseEvent(event);
  }
}

void Viewport::mouseDoubleClickEvent(QMouseEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->MouseDoubleClickEvent(event);
  }
}

void Viewport::wheelEvent(QWheelEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->WheelEvent(event);
  }
}

void Viewport::keyPressEvent(QKeyEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->KeyPressEvent(event);
  } else {
    event->ignore();
  }
}

void Viewport::keyReleaseEvent(QKeyEvent* event) {
  if (p_->input_handler) {
    makeCurrent();
    p_->input_handler->KeyReleaseEvent(event);
  }
}

void Viewport::Render() { repaint(0, 0, width(), height()); }

}  // namespace sv
