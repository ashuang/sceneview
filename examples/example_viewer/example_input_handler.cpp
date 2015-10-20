#include "example_input_handler.hpp"

#include <QTimer>

namespace sv = sceneview;
using sv::CameraNode;
using sv::MaterialResource;
using sv::StockResources;
using sv::Scene;

static const int kTimerInveralMs = 10;

namespace vis_examples {

ExampleInputHandler::ExampleInputHandler(sv::Viewport* viewport,
    QObject* parent) :
  QObject(parent),
  viewport_(viewport),
  shape_(nullptr),
  shape_speed_(5.0),
  timer_(new QTimer(this)) {
  timer_->setInterval(kTimerInveralMs);
  connect(timer_, &QTimer::timeout, this, &ExampleInputHandler::Update);
}

QWidget* ExampleInputHandler::GetWidget() {
  return nullptr;
}

void ExampleInputHandler::InitializeGL() {
  Scene::Ptr scene = viewport_->GetScene();

  StockResources stock(viewport_->GetResources());
  MaterialResource::Ptr material =
    stock.NewMaterial(StockResources::kUniformColorLighting);
  material->SetParam("diffuse", 1.0, 0.2, 1.0, 0.75);

  shape_ = scene->MakeMesh(scene->Root());
  shape_->Add(stock.Sphere(), material);
  shape_->SetVisible(false);
}

void ExampleInputHandler::ShutdownGL() {

}

void ExampleInputHandler::Activated() {

}

void ExampleInputHandler::Deactivated() {
  shape_->SetVisible(false);
  timer_->stop();
}

void ExampleInputHandler::MousePressEvent(QMouseEvent* event) {
  CameraNode* camera = viewport_->GetCamera();
  shape_dir_ = camera->Unproject(event->x(), event->y()).normalized();
  shape_pos_ = camera->Translation() + shape_dir_;

  shape_->SetTranslation(shape_pos_);
  timer_->start();
  shape_->SetVisible(true);
}

void ExampleInputHandler::MouseMoveEvent(QMouseEvent* event) {

}

void ExampleInputHandler::MouseReleaseEvent(QMouseEvent* event) {

}

void ExampleInputHandler::WheelEvent(QWheelEvent* event) {

}

void ExampleInputHandler::KeyPressEvent(QKeyEvent* event) {

}

void ExampleInputHandler::KeyReleaseEvent(QKeyEvent* event) {

}

void ExampleInputHandler::Update() {
  const double dt = static_cast<float>(kTimerInveralMs) / 1000;
  shape_pos_ += shape_dir_ * shape_speed_ * dt;
  shape_->SetTranslation(shape_pos_);
  viewport_->ScheduleRedraw();
}

}  // namespace vis_examples
