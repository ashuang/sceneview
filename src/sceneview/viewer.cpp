#include <sceneview/viewer.hpp>

#include <QAction>
#include <QCoreApplication>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>

#include <sceneview/input_handler_widget.hpp>
#include <sceneview/renderer_widget_stack.hpp>
#include <sceneview/view_handler_horizontal.hpp>

namespace sceneview {

Viewer::Viewer(QWidget* parent) :
  QMainWindow(parent),
  resources_(ResourceManager::Create()),
  default_scene_(resources_->MakeScene("default_scene")),
  viewport_(new Viewport(resources_, default_scene_, this)) {

  // Add a couple lights
  LightNode* light0 = default_scene_->MakeLight(default_scene_->Root());
  light0->SetDirection(QVector3D(1, 1, -1));
  light0->SetLightType(LightType::kDirectional);
  light0->SetAmbient(0.05);

  // Add a camera to the scene
  CameraNode* camera = default_scene_->MakeCamera(default_scene_->Root());

  // Set the camera perspective projection parameters, and point it somewhere.
  const QVector3D eye(5, 5, -10);
  const QVector3D look_at(0, 0, 0);
  const QVector3D up(0, 0, -1);
  camera->LookAt(eye, look_at, up);
  camera->SetProjectionParams(CameraNode::kPerspective, 50, 0.1, 5000);

  // Assign the camera to the viewport
  viewport_->SetCamera(camera);

  // Add a renderer widget stack
  renderer_widget_stack_ = new RendererWidgetStack(this);

  // Whenever a renderer is added to the viewport, add its widget to
  // the renderer widget stack and a menu entry.
  connect(viewport_, &Viewport::RendererAdded,
      this, &Viewer::OnRendererAdded);

  // Similarly, whenever an input handler is added to the viewport, add
  // a menu entry.
  connect(viewport_, &Viewport::InputHandlerAdded,
      this, &Viewer::OnInputHandlerAdded);
  connect(viewport_, &Viewport::InputHandlerActivated,
      this, &Viewer::OnInputHandlerActivated);

  setCentralWidget(viewport_);

  addDockWidget(Qt::LeftDockWidgetArea, renderer_widget_stack_);

  // Add the input controls dock widget
  input_handler_widget_ = new InputHandlerWidget(viewport_, this);

  addDockWidget(Qt::LeftDockWidgetArea, input_handler_widget_);

  CreateMenus();

  resize(800, 600);
}

void Viewer::SetAutoRedrawInterval(int milliseconds) {
  redraw_timer_.setInterval(50);
  connect(&redraw_timer_, &QTimer::timeout,
      viewport_, &Viewport::ScheduleRedraw);
  redraw_timer_.start();
}

void Viewer::SaveSettings(QSettings* settings) {
  settings->beginGroup("viewer");
  settings->setValue("geometry", saveGeometry());
  settings->setValue("windowState", saveState());
  settings->endGroup();

  // Renderers
  settings->beginGroup("renderers");
  for (Renderer* renderer : viewport_->GetRenderers()) {
    const QString name = renderer->Name();
    settings->setValue(name, renderer->SaveState());
  }
  settings->endGroup();

  // Input handlers
  settings->beginGroup("input_handlers");
  for (InputHandler* handler : viewport_->GetInputHandlers()) {
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
  for (Renderer* renderer : viewport_->GetRenderers()) {
    const QString name = renderer->Name();
    if (settings->contains(name)) {
        renderer->LoadState(settings->value(name));
    }
  }
  settings->endGroup();

  // Input handlers
  settings->beginGroup("input_handlers");
  for (InputHandler* handler : viewport_->GetInputHandlers()) {
    const QString& name = handler->Name();
    if (settings->contains(name)) {
        handler->LoadState(settings->value(name));
    }
  }
  settings->endGroup();
}

void Viewer::OnRendererAdded(Renderer* renderer) {
  QAction* enable_disable =
    renderer_menu_->addAction(renderer->Name());
  enable_disable->setCheckable(true);
  enable_disable->setChecked(true);
  connect(enable_disable, &QAction::toggled, renderer, &Renderer::SetEnabled);

  renderer_widget_stack_->AddRendererWidget(renderer);
}

void Viewer::OnInputHandlerAdded(InputHandler* handler) {
  // Add a menu item to the "Input" menu
  QAction* action = input_handler_menu_->addAction(handler->Name());
  action->setCheckable(true);
  action->setChecked(false);
  input_action_group_->addAction(action);

  input_handler_actions_[handler] = action;

  // When the item is activated, command the viewport to activate the input handler.
  connect(action, &QAction::toggled,
      [this, handler](bool checked) {
        if (checked) {
          viewport_->ActivateInputHandler(handler);
        }});
}

void Viewer::OnInputHandlerActivated(InputHandler* handler) {
  auto iter = input_handler_actions_.find(handler);
  if (iter == input_handler_actions_.end()) {
    return;
  }
  QAction* action = iter->second;
  action->setChecked(true);
}

void Viewer::CreateMenus() {
  // Setup file menu
  file_menu_ = menuBar()->addMenu("&File");
  QAction* quit_action = file_menu_->addAction("&Quit");
  quit_action->setShortcut(QKeySequence::Quit);
  connect(quit_action, &QAction::triggered, this, &QMainWindow::close);

  // Setup renderer menu
  renderer_menu_ = menuBar()->addMenu("&Renderers");

  // Setup view menu
  view_menu_ = menuBar()->addMenu("&View");
  QAction* show_renderer_widgets = view_menu_->addAction("&Renderer widgets");
  SetupShowHideAction(show_renderer_widgets, renderer_widget_stack_);

  QAction* show_input_handler = view_menu_->addAction("&Input handler");
  SetupShowHideAction(show_input_handler, input_handler_widget_);

  // Setup input handler menu
  input_handler_menu_ = menuBar()->addMenu("&Input");
  input_action_group_ = new QActionGroup(this);
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

}  // namespace sceneview
