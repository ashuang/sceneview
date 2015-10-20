#include <QApplication>
#include <QSettings>

#include <sceneview/sceneview.hpp>

#include "example_input_handler.hpp"
#include "stock_shapes_renderer.hpp"
#include "fixed_function_renderer.hpp"
#include "text_renderer.hpp"

namespace sv = sceneview;
using sv::Viewport;
using sv::Viewer;
using sv::GridRenderer;
using sv::ViewHandlerHorizontal;

using vis_examples::ExampleInputHandler;
using vis_examples::StockShapesRenderer;
using vis_examples::FixedFunctionRenderer;
using vis_examples::TextRenderer;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Viewer viewer;

  Viewport* viewport = viewer.GetViewport();

  // Set initial camera position
  const QVector3D eye(5, 5, -10);
  const QVector3D look_at(0, 0, 0);
  const QVector3D up(0, 0, -1);
  viewport->GetCamera()->LookAt(eye, look_at, up);

  // Add renderers
  viewport->AddRenderer(new GridRenderer("grid", viewport));
  viewport->AddRenderer(new StockShapesRenderer("example", viewport));
  viewport->AddRenderer(new FixedFunctionRenderer("fixed_function", viewport));
  viewport->AddRenderer(new TextRenderer("text", viewport));

  // Add input handlers
  viewport->AddInputHandler(new ViewHandlerHorizontal(viewport,
      QVector3D(0, 0, -1), viewport));
  viewport->AddInputHandler(new ExampleInputHandler(viewport, viewport));

  // Renderers and input handlers are all created.
  // Load any previously saved settings.
  QSettings settings("vis_examples", "sv_example_viewer");
  viewer.LoadSettings(&settings);

  // Connect to the viewer "Closing" signal to save the viewer state on exit
  QObject::connect(&viewer, &Viewer::Closing,
      [&viewer]() {
        QSettings settings("vis_examples", "sv_example_viewer");
        viewer.SaveSettings(&settings);
      });

  // Go
  viewer.SetAutoRedrawInterval(60);
  viewer.show();
  return app.exec();
}
