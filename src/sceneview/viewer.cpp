// Copyright [2015] Albert Huang

#include "sceneview/viewer.hpp"

#include <QAction>
#include <QCoreApplication>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>

#include "sceneview/camera_node.hpp"
#include "sceneview/input_handler_widget_stack.hpp"
#include "sceneview/light_node.hpp"
#include "sceneview/renderer.hpp"
#include "sceneview/renderer_widget_stack.hpp"
#include "sceneview/view_handler_horizontal.hpp"

namespace sv {

struct Viewer::Priv {
  ResourceManager::Ptr resources;
  Scene::Ptr default_scene;
  Viewport* viewport;

  RendererWidgetStack* renderer_widget_stack;
  InputHandlerWidgetStack* input_handler_widget;

  QTimer redraw_timer;

  QMenu* file_menu;
  QMenu* renderer_menu;
  QMenu* view_menu;
  QMenu* input_handler_menu;
  QActionGroup* input_action_group;
  std::map<InputHandler*, QAction*> input_handler_actions;
};

Viewer::Viewer(QWidget* parent) :
  QMainWindow(parent),
  p_(new Priv) {
  p_->resources = ResourceManager::Create();
  p_->default_scene = p_->resources->MakeScene("default_scene");
  p_->viewport = new Viewport(p_->resources, p_->default_scene, this);

  // Add a couple lights
  LightNode* light0 = p_->default_scene->MakeLight(p_->default_scene->Root());
  light0->SetDirection(QVector3D(1, 1, -1));
  light0->SetLightType(LightType::kDirectional);
  light0->SetAmbient(0.05);

  // Add a camera to the scene
  CameraNode* camera = p_->default_scene->MakeCamera(p_->default_scene->Root());

  // Set the camera perspective projection parameters, and point it somewhere.
  const QVector3D eye(5, 5, -10);
  const QVector3D look_at(0, 0, 0);
  const QVector3D up(0, 0, -1);
  camera->LookAt(eye, look_at, up);
  camera->SetPerspective(50, 0.1, 5000);

  // Assign the camera to the viewport
  p_->viewport->SetCamera(camera);

  // Add a renderer widget stack
  p_->renderer_widget_stack = new RendererWidgetStack(this);

  // Whenever a renderer is added to the viewport, add its widget to
  // the renderer widget stack and a menu entry.
  connect(p_->viewport, &Viewport::RendererAdded,
      this, &Viewer::OnRendererAdded);

  // Similarly, whenever an input handler is added to the viewport, add
  // a menu entry.
  connect(p_->viewport, &Viewport::InputHandlerAdded,
      this, &Viewer::OnInputHandlerAdded);
  connect(p_->viewport, &Viewport::InputHandlerActivated,
      this, &Viewer::OnInputHandlerActivated);

  setCentralWidget(p_->viewport);

  addDockWidget(Qt::LeftDockWidgetArea, p_->renderer_widget_stack);

  // Add the input controls dock widget
  p_->input_handler_widget = new InputHandlerWidgetStack(p_->viewport, this);

  addDockWidget(Qt::LeftDockWidgetArea, p_->input_handler_widget);

  CreateMenus();

  resize(800, 600);
}

Viewer::~Viewer()
{
  delete p_;
}

Viewport* Viewer::GetViewport() { return p_->viewport; }

void Viewer::SetAutoRedrawInterval(int milliseconds) {
  p_->redraw_timer.setInterval(milliseconds);
  connect(&p_->redraw_timer, &QTimer::timeout,
      p_->viewport, &Viewport::ScheduleRedraw);
  p_->redraw_timer.start();
}

QMenu* Viewer::FileMenu() { return p_->file_menu; }

void Viewer::SaveSettings(QSettings* settings) {
  settings->beginGroup("viewer");
  settings->setValue("geometry", saveGeometry());
  settings->setValue("windowState", saveState());
  settings->endGroup();

  // Renderers
  settings->beginGroup("renderers");
  for (Renderer* renderer : p_->viewport->GetRenderers()) {
    const QString name = renderer->Name();
    settings->setValue(name, renderer->SaveState());
  }
  settings->endGroup();

  // Input handlers
  settings->beginGroup("input_handlers");
  for (InputHandler* handler : p_->viewport->GetInputHandlers()) {
    settings->setValue(handler->Name(), handler->SaveState());
  }
  settings->endGroup();
}

void Viewer::LoadSettings(QSettings* settings) {
  settings->beginGroup("viewer");
  if (settings->contains("windowState")) {
    restoreState(settings->value("windowState").toByteArray());
  }
  if (settings->contains("geometry")) {
    restoreGeometry(settings->value("geometry").toByteArray());
  }
  settings->endGroup();

  // Renderers
  settings->beginGroup("renderers");
  for (Renderer* renderer : p_->viewport->GetRenderers()) {
    const QString name = renderer->Name();
    if (settings->contains(name)) {
        renderer->LoadState(settings->value(name));
    }
  }
  settings->endGroup();

  // Input handlers
  settings->beginGroup("input_handlers");
  for (InputHandler* handler : p_->viewport->GetInputHandlers()) {
    const QString& name = handler->Name();
    if (settings->contains(name)) {
        handler->LoadState(settings->value(name));
    }
  }
  settings->endGroup();
}

RendererWidgetStack* Viewer::GetRendererWidgetStack() { return p_->renderer_widget_stack; }

void Viewer::OnRendererAdded(Renderer* renderer) {
  QAction* enable_disable =
    p_->renderer_menu->addAction(renderer->Name());
  enable_disable->setCheckable(true);
  enable_disable->setChecked(true);
  connect(enable_disable, &QAction::toggled, renderer, &Renderer::SetEnabled);
  connect(renderer, &Renderer::EnableChanged, enable_disable,
      &QAction::setChecked);

  p_->renderer_widget_stack->AddRendererWidget(renderer);
}

void Viewer::OnInputHandlerAdded(InputHandler* handler) {
  // Add a menu item to the "Input" menu
  QAction* action = p_->input_handler_menu->addAction(handler->Name());
  action->setCheckable(true);
  action->setChecked(false);
  p_->input_action_group->addAction(action);

  p_->input_handler_actions[handler] = action;

  // When the item is activated, command the viewport to activate the input
  // handler.
  connect(action, &QAction::toggled,
      [this, handler](bool checked) {
        if (checked) {
          p_->viewport->ActivateInputHandler(handler);
        }});
}

void Viewer::OnInputHandlerActivated(InputHandler* handler) {
  auto iter = p_->input_handler_actions.find(handler);
  if (iter == p_->input_handler_actions.end()) {
    return;
  }
  QAction* action = iter->second;
  action->setChecked(true);
}

void Viewer::CreateMenus() {
  // Setup file menu
  p_->file_menu = menuBar()->addMenu("&File");
  QAction* quit_action = p_->file_menu->addAction("&Quit");
  quit_action->setShortcut(QKeySequence::Quit);
  connect(quit_action, &QAction::triggered, this, &QMainWindow::close);

  // Setup renderer menu
  p_->renderer_menu = menuBar()->addMenu("&Renderers");

  // Setup view menu
  p_->view_menu = menuBar()->addMenu("&View");
  QAction* show_renderer_widgets = p_->view_menu->addAction("&Renderer widgets");
  SetupShowHideAction(show_renderer_widgets, p_->renderer_widget_stack);

  QAction* show_input_handler = p_->view_menu->addAction("&Input handler");
  SetupShowHideAction(show_input_handler, p_->input_handler_widget);

  // Setup input handler menu
  p_->input_handler_menu = menuBar()->addMenu("&Input");
  p_->input_action_group = new QActionGroup(this);
}

void Viewer::closeEvent(QCloseEvent* event) {
  emit Closing();
}

void Viewer::SetupShowHideAction(QAction* action, QDockWidget* widget) {
  action->setCheckable(true);
  action->setChecked(true);

  // Show / hide the renderer widgets when toggle_view_renderer_widgets_action
  // is toggled
  connect(action, &QAction::toggled, widget, &QDockWidget::setVisible);

  // Conversely, toggle the menu checkmark when the renderer stack dockwidget
  // is manually closed
  connect(widget, &QDockWidget::visibilityChanged,
      action, &QAction::setChecked);
}

}  // namespace sv
