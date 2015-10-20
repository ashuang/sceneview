#include "stock_shapes_renderer.hpp"

#include <cmath>

#include <sceneview/stock_resources.hpp>

using sv::GroupNode;
using sv::MeshNode;
using sv::ParamWidget;
using sv::Renderer;
using sv::ResourceManager;
using sv::Scene;
using sv::StockResources;

namespace vis_examples {

StockShapesRenderer::StockShapesRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  start_time_(QTime::currentTime()),
  angle_(0) {
}

void StockShapesRenderer::InitializeGL() {
  Scene::Ptr scene = GetScene();
  ResourceManager::Ptr resources = GetResources();
  GroupNode* base_node = GetBaseNode();

  // Create a material to share among several shapes
  StockResources stock(resources);
  material_ = stock.NewMaterial(StockResources::kUniformColorLighting);
  material_->SetParam(sv::kDiffuse, 1.0, 0.5, 0.5, 1.0);
  material_->SetParam(sv::kSpecular, 1.0, 0.5, 0.5, 1.0);
  material_->SetParam(sv::kShininess, 10.0f);

  // Create a bunch of shapes
  shapes_.push_back(scene->MakeMesh(base_node, stock.Cone(), material_));
  shapes_.push_back(scene->MakeMesh(base_node, stock.Cube(), material_));
  shapes_.push_back(scene->MakeMesh(base_node, stock.Cylinder(), material_));
  shapes_.push_back(scene->MakeMesh(base_node, stock.Sphere(), material_));

  MeshNode* axes = scene->MakeMesh(base_node);
  axes->Add(stock.UnitAxes());
  shapes_.push_back(axes);

  // Move the shapes so they're not all on top of each other.
  const double spacing = 2.0;
  const double x_start = - spacing * shapes_.size() / 2;
  for (size_t i = 0; i < shapes_.size(); ++i) {
    shapes_[i]->SetTranslation(x_start + i * spacing, 0, 0);
  }
}

void StockShapesRenderer::RenderBegin() {
  const double elapsed = start_time_.restart() / 1000.;
  const double speed = widget_->GetDouble("speed");
  angle_ += elapsed * speed;
  const QQuaternion rot = QQuaternion::fromAxisAndAngle(
      QVector3D(1, 0, 0), angle_ * 180 / M_PI);

  for (MeshNode* shape : shapes_) {
    shape->SetRotation(rot);
  }

  material_->SetParam(sv::kDiffuse, sin(angle_), 0.5, 0.5, 1.0);
}

QWidget* StockShapesRenderer::GetWidget() {
  if (widget_) {
    return widget_.get();
  }

  ParamWidget* pwidget = new ParamWidget();
  pwidget->AddBoolean("spin", true, ParamWidget::kCheckBox);
  pwidget->AddDouble("speed", 0, 2.0, 0.1, 1.0, ParamWidget::kSlider);
  pwidget->AddDouble("double-spinbox", 0, 2.0, 0.1, 1.0, ParamWidget::kSpinBox);
  pwidget->AddEnum("enum",
      {{"first", 1},
       {"second", 2},
       {"third", 3}},
       2, ParamWidget::kComboBox);
  pwidget->AddInt("int-slider", 0, 100, 1, 50, ParamWidget::kSlider);
  pwidget->AddInt("int-spinbox", 0, 100, 1, 50, ParamWidget::kSpinBox);
  connect(pwidget, &ParamWidget::ParamChanged, this, &StockShapesRenderer::ParamChanged);
  widget_.reset(pwidget);
  return pwidget;
}

QVariant StockShapesRenderer::SaveState() {
  return widget_->SaveState();
}

void StockShapesRenderer::LoadState(const QVariant& val) {
  widget_->LoadState(val);
}

void StockShapesRenderer::ParamChanged(const QString& name) {
  if (name == "enum") {
    printf("enum: %d\n", widget_->GetEnum(name));
  } else if (name == "double-spinbox") {
    printf("double-spinbox: %f\n", widget_->GetDouble(name));
  } else if (name == "spin") {
    if (widget_->GetBool(name)) {
      start_time_.start();
    }
  } else if (name == "int-slider") {
    printf("int-slider value changed to %d\n", widget_->GetInt(name));
  } else if (name == "int-spinbox") {
    printf("int-spinbox value changed to %d\n", widget_->GetInt(name));
  }
}

}  // namespace vis_examples
