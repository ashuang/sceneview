#include "example_renderer.hpp"

#include <cmath>

#include <sceneview/stock_resources.hpp>

namespace sv = sceneview;
using sv::ParamWidget;
using sv::MeshNode;
using sv::Renderer;
using sv::Scene;
using sv::GroupNode;
using sv::ResourceManager;
using sv::StockResources;
using sv::MaterialResource;

namespace vis_examples {

ExampleRenderer::ExampleRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  start_time_(QTime::currentTime()),
  angle_(0) {
}

void ExampleRenderer::InitializeGL() {
  Scene::Ptr scene = GetScene();
  ResourceManager::Ptr resources = GetResources();
  GroupNode* base_node = GetBaseNode();

  // Create a material to share among several shapes
  StockResources stock(resources);
  material_ = stock.NewMaterial(StockResources::kUniformColorLighting);
  material_->SetParam("diffuse", 1.0, 0.5, 0.5, 1.0);
  material_->SetParam("specular", 1.0, 0.5, 0.5, 1.0);
  material_->SetParam("shininess", 10.0f);

  // Create a bunch of shapes
  MeshNode* cone = scene->MakeMesh(base_node);
  MeshNode* cube = scene->MakeMesh(base_node);
  MeshNode* cylinder = scene->MakeMesh(base_node);
  MeshNode* sphere = scene->MakeMesh(base_node);
  MeshNode* axes = scene->MakeMesh(base_node);

  cone->Add(stock.Cone(), material_);
  cube->Add(stock.Cube(), material_);
  cylinder->Add(stock.Cylinder(), material_);
  sphere->Add(stock.Sphere(), material_);
  axes->Add(stock.UnitAxes());

  shapes_.push_back(cone);
  shapes_.push_back(cube);
  shapes_.push_back(cylinder);
  shapes_.push_back(sphere);
  shapes_.push_back(axes);

  const double spacing = 2.0;
  const double x_start = - spacing * shapes_.size() / 2;
  for (size_t i = 0; i < shapes_.size(); ++i) {
    shapes_[i]->SetTranslation(x_start + i * spacing, 0, 0);
  }
}

void ExampleRenderer::RenderBegin() {
  const double elapsed = start_time_.restart() / 1000.;
  const double speed = widget_->GetDouble("speed");
  angle_ += elapsed * speed;
  const QQuaternion rot = QQuaternion::fromAxisAndAngle(
      QVector3D(1, 0, 0), angle_ * 180 / M_PI);

  for (MeshNode* shape : shapes_) {
    shape->SetRotation(rot);
  }

  material_->SetParam("diffuse", sin(angle_), 0.5, 0.5, 1.0);
}

QWidget* ExampleRenderer::GetWidget() {
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
  connect(pwidget, &ParamWidget::ParamChanged, this, &ExampleRenderer::ParamChanged);
  widget_.reset(pwidget);
  return pwidget;
}

QVariant ExampleRenderer::SaveState() {
  return widget_->SaveState();
}

void ExampleRenderer::LoadState(const QVariant& val) {
  widget_->LoadState(val);
}

void ExampleRenderer::ParamChanged(const QString& name) {
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
