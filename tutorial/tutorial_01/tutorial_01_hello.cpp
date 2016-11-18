#include <QApplication>

#include <sceneview/sceneview.hpp>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // Create a sv::Viewer. This is a convenience widget that extends QMainWindow
  // and creates a "stock" application with the following contained widgets:
  // - sv::Viewport
  // - sv::RendererWidgetStack
  // - sv::InputHandlerWidgetStack
  sv::Viewer viewer;

  // Show the stock viewer.
  viewer.show();

  // Run the Qt event loop
  return app.exec();
}
