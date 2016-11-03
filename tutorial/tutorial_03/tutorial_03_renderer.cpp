#include <QApplication>

#include <sceneview/sceneview.hpp>

#include "my_renderer.hpp"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // Create a sv::Viewer. This is a convenience widget that extends QMainWindow
  // and creates a "stock" application with the following contained widgets:
  // - sv::Viewport
  // - sv::RendererWidgetStack
  // - sv::InputHandlerWidgetStack
  sv::Viewer viewer;

  // Get the viewer's viewport widget.
  sv::Viewport* viewport = viewer.GetViewport();

  // Add a builtin grid renderer to the viewer's viewport.
  sv::Renderer* grid_renderer = new sv::GridRenderer("grid", viewport);
  viewport->AddRenderer(grid_renderer);

  // Add a custom renderer
  sv::Renderer* my_renderer = new MyRenderer("my_renderer", viewport);
  viewport->AddRenderer(my_renderer);

  // Add a builtin input handler that controls the camera position and
  // orientation using mouse and keyboard input events.
  sv::InputHandler* input_handler = new sv::ViewHandlerHorizontal(viewport,
      QVector3D(0, 0, -1), viewport);
  viewport->AddInputHandler(input_handler);

  // Show the viewer.
  viewer.show();

  // Run the Qt event loop.
  return app.exec();
}
