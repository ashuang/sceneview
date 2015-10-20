#include "model_renderer.hpp"

#include <QFileDialog>

using sv::AssetImporter;
using sv::Renderer;
using sv::ParamWidget;
using sv::Scene;
using sv::AxisAlignedBox;

namespace vis_examples {

ModelRenderer::ModelRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  node_(nullptr),
  gl_initialized_(false),
  model_fname_("") {
  params_.reset(new ParamWidget());
  params_->AddPushButton("Load");
  params_->AddPushButton("Clear");
  params_->SetEnabled("Load", false);
  connect(params_.get(), &ParamWidget::ParamChanged,
      this, &ModelRenderer::ParamChanged);
}

void ModelRenderer::InitializeGL() {
  params_->SetEnabled("Load", true);
  gl_initialized_ = true;
  if (!model_fname_.isEmpty()) {
    LoadModelGL();
  }
}

void ModelRenderer::ShutdownGL() {
  params_->SetEnabled("Load", false);
  ClearModel();
  gl_initialized_ = false;
}

void ModelRenderer::LoadModel(const QString& filename) {
  if (gl_initialized_) {
    model_fname_ = filename;
    LoadModelGL();
  } else {
    model_fname_ = filename;
  }
}

void ModelRenderer::ParamChanged(const QString& name) {
  if (name == "Load") {
    QString fname = QFileDialog::getOpenFileName(params_.get(), "Load File");
    if (fname.isEmpty()) {
      return;
    }

    LoadModel(fname);
  } else if (name == "Clear") {
    ClearModel();
  }
}

void ModelRenderer::LoadModelGL() {
  printf("Loading %s\n", model_fname_.toStdString().c_str());
  Scene::Ptr scene = GetScene();

  // Clear out any previously loaded model.
  ClearModel();

  // Load the model as a scene graph resource.
  Scene::Ptr model = AssetImporter::ImportFile(GetResources(), model_fname_);

  if (!model) {
    return;
  }

  // Create a node in the main scene graph.
  node_ = scene->MakeGroup(GetBaseNode());

  // Instantiate the model as a child of the newly created node.
  scene->MakeGroupFromScene(node_, model);

  // Scale and translate the model so that it fits inside a unit cube
  // centered at the origin.
  const AxisAlignedBox box = node_->BoundingBox();
  const QVector3D span = box.Max() - box.Min();
  const float max_span = std::max(std::max(span.x(), span.y()), span.z());
  const double scale_factor = 1.0 / max_span;
  node_->SetScale(QVector3D(scale_factor, scale_factor, scale_factor));
  node_->SetTranslation(-0.5 * scale_factor * (box.Max() + box.Min()));

  GetViewport()->ScheduleRedraw();
}

void ModelRenderer::ClearModel() {
  if (!node_) {
    return;
  }

  GetScene()->DestroyNode(node_);
  node_ = nullptr;
}

}  // namespace vis_examples
