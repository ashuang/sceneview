#include <QApplication>
#include <QSettings>

#include <sceneview/sceneview.hpp>

#include "example_input_handler.hpp"
#include "fixed_function_renderer.hpp"
#include "polylines_renderer.hpp"
#include "stock_shape_renderer.hpp"
#include "stock_shape_selector.hpp"
#include "text_renderer.hpp"
#include "texture_renderer.hpp"
#include "hud_renderer.hpp"

using sv::Viewport;
using sv::Viewer;
using sv::GridRenderer;
using sv::ViewHandlerHorizontal;

using vis_examples::ExampleInputHandler;
using vis_examples::StockShapeRenderer;
using vis_examples::StockShapeSelector;
using vis_examples::FixedFunctionRenderer;
using vis_examples::PolylinesRenderer;
using vis_examples::TextRenderer;
using vis_examples::TextureRenderer;
using vis_examples::HudRenderer;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Viewer viewer;

  Viewport* viewport = viewer.GetViewport();

  // Set initial camera position
  const QVector3D eye(5, 5, -10);
  const QVector3D look_at(0, 0, 0);
  const QVector3D up(0, 0, -1);
  viewport->GetCamera()->LookAt(eye, look_at, up);

  // Setup the HUD
  sv::Scene::Ptr scene = viewport->GetScene();
  sv::DrawGroup* hud_group = scene->MakeDrawGroup(
      sv::Scene::kDefaultDrawGroupOrder + 10, "HUD");
  hud_group->SetFrustumCulling(false);
  hud_group->SetNodeOrdering(sv::NodeOrdering::kNone);
  viewport->SetDrawGroups({scene->GetDefaultDrawGroup(), hud_group });

  sv::CameraNode* hud_cam = scene->MakeCamera(scene->Root());
  hud_group->SetCamera(hud_cam);
  auto on_viewport_resize = [viewport, hud_cam]() {
      const int width = viewport->width();
      const int height = viewport->height();
      QMatrix4x4 mvp_mat;
      mvp_mat.ortho(0, width, height, 0, -1, 1);
      hud_cam->SetManual(mvp_mat);
      };
  QObject::connect(viewport, &sv::Viewport::resized, on_viewport_resize);

  // Add renderers
  viewport->AddRenderer(new GridRenderer("grid", viewport));
  StockShapeRenderer* stock_shape_renderer =
    new StockShapeRenderer("example", viewport);
  viewport->AddRenderer(stock_shape_renderer);
  viewport->AddRenderer(new FixedFunctionRenderer("fixed_function", viewport));
  viewport->AddRenderer(new TextRenderer("text", viewport));
  viewport->AddRenderer(new PolylinesRenderer("polylines", viewport));
  viewport->AddRenderer(new TextureRenderer("texture", viewport));
  viewport->AddRenderer(new HudRenderer("hud", viewport));

  // Add input handlers
  viewport->AddInputHandler(new ViewHandlerHorizontal(viewport,
      QVector3D(0, 0, -1), viewport));
  viewport->AddInputHandler(new ExampleInputHandler(viewport, viewport));
  viewport->AddInputHandler(new StockShapeSelector(stock_shape_renderer,
        viewport));

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
