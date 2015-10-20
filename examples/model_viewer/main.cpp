#include <QApplication>

#include <sceneview/grid_renderer.hpp>
#include <sceneview/viewer.hpp>
#include <sceneview/view_handler_horizontal.hpp>

#include "model_renderer.hpp"

namespace sv = sceneview;
using sv::Viewport;
using sv::Viewer;
using sv::GridRenderer;
using sv::ViewHandlerHorizontal;

using vis_examples::ModelRenderer;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Viewer viewer;

  Viewport* viewport = viewer.GetViewport();

  // Set initial camera position
  const QVector3D eye(5, 5, -10);
  const QVector3D look_at(0, 0, 0);
  const QVector3D up(0, 1, 0);
  viewport->GetCamera()->LookAt(eye, look_at, up);

  // Add renderers
  ModelRenderer* modeler = new ModelRenderer("model", viewport);
  viewport->AddRenderer(modeler);

  if (argc > 1) {
    modeler->LoadModel(argv[1]);
  }

  // Add input handlers
  viewport->AddInputHandler(new ViewHandlerHorizontal(viewport,
      QVector3D(0, 1, 0), viewport));

  viewer.show();

  return app.exec();
}
