#include "stock_shape_renderer.hpp"

#include <cmath>

#include <sceneview/scene_node.hpp>
#include <sceneview/stock_resources.hpp>

using sv::GroupNode;
using sv::Drawable;
using sv::DrawNode;
using sv::ParamWidget;
using sv::Renderer;
using sv::ResourceManager;
using sv::Scene;
using sv::StockResources;
using sv::SceneNode;

namespace vis_examples {

StockShapeRenderer::StockShapeRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  selected_node_(nullptr),
  start_time_(QTime::currentTime()),
  angle_(0) {
}

void StockShapeRenderer::InitializeGL() {
  Scene::Ptr scene = GetScene();
  ResourceManager::Ptr resources = GetResources();
  GroupNode* base_node = GetBaseNode();

  // Create a material to share among several shapes
  StockResources stock(resources);
  material_ = stock.NewMaterial(StockResources::kUniformColorLighting);
  material_->SetParam(sv::kDiffuse, 1.0, 0.5, 0.5, 1.0);
  material_->SetParam(sv::kSpecular, 1.0, 0.5, 0.5, 1.0);
  material_->SetParam(sv::kShininess, 10.0f);

  // Create a material to use for the selected shape
  select_material_ = stock.NewMaterial(StockResources::kUniformColorLighting);
  select_material_->SetParam(sv::kDiffuse, 1.0, 0.0, 1.0, 1.0);
  select_material_->SetParam(sv::kSpecular, 1.0, 0.0, 0.1, 1.0);
  select_material_->SetParam(sv::kShininess, 16.0f);

  // Create a bunch of shapes
  shapes_.push_back(scene->MakeDrawNode(base_node, stock.Cone(), material_));
  shapes_.push_back(scene->MakeDrawNode(base_node, stock.Cube(), material_));
  shapes_.push_back(scene->MakeDrawNode(base_node, stock.Cylinder(), material_));
  shapes_.push_back(scene->MakeDrawNode(base_node, stock.Sphere(), material_));

  DrawNode* axes = scene->MakeDrawNode(base_node);
  axes->Add(stock.UnitAxes());
  shapes_.push_back(axes);

  // Move the shapes so they're not all on top of each other.
  //
  // Also set the selection mask for each node so that they can be selected by
  // StockShapeSelector
  const double spacing = 2.0;
  const double x_start = - spacing * shapes_.size() / 2;
  for (size_t i = 0; i < shapes_.size(); ++i) {
    shapes_[i]->SetTranslation(x_start + i * spacing, 0, 0);
    shapes_[i]->SetSelectionMask(1);
  }
}

void StockShapeRenderer::RenderBegin() {
  const double elapsed = start_time_.restart() / 1000.;
  const double speed = widget_->GetDouble("speed");
  angle_ += elapsed * speed;
  const QQuaternion rot = QQuaternion::fromAxisAndAngle(
      QVector3D(1, 0, 0), angle_ * 180 / M_PI);

  for (DrawNode* shape : shapes_) {
    shape->SetRotation(rot);
  }

  material_->SetParam(sv::kDiffuse, sin(angle_), 0.5, 0.5, 1.0);
}

QWidget* StockShapeRenderer::GetWidget() {
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
  connect(pwidget, &ParamWidget::ParamChanged, this, &StockShapeRenderer::ParamChanged);
  widget_.reset(pwidget);
  return pwidget;
}

QVariant StockShapeRenderer::SaveState() {
  return widget_->SaveState();
}

void StockShapeRenderer::LoadState(const QVariant& val) {
  widget_->LoadState(val);
}

// Called by StockShapeSelector when StockShapeSelector is the active input
// handler and the user has clicked on one of the stock shapes.
void StockShapeRenderer::NodeSelected(SceneNode* node) {
  // Find the selected shape
  auto iter = std::find(shapes_.begin(), shapes_.end(),
      dynamic_cast<DrawNode*>(node));
  if (iter == shapes_.end()) {
    return;
  }

  DrawNode* draw_node = *iter;
  if (draw_node == selected_node_) {
    return;
  }

  // If there was a previously selected shape, then restore that shape's
  // material.
  //
  // Change the material for the shape's first Drawable. This works since the
  // stock shapes only have one material. For more complex shapes (e.g., 3D
  // models loaded through AssetImporter), it's not as easy to just swap
  // materials in and out without knowing more about the exact shape.
  if (selected_node_) {
    Drawable::Ptr first_drawable = selected_node_->Drawables().front();
    first_drawable->SetMaterial(selected_orig_material_);
  }

  // Set the new selected node, and change its material.
  selected_node_ = draw_node;
  Drawable::Ptr first_drawable = selected_node_->Drawables().front();
  selected_orig_material_ = first_drawable->Material();
  first_drawable->SetMaterial(select_material_);
}

void StockShapeRenderer::ParamChanged(const QString& name) {
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
