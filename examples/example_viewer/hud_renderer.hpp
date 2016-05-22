#ifndef SCENEVIEW_EXAMPLES_HUD_RENDERER_HPP__
#define SCENEVIEW_EXAMPLES_HUD_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

namespace vis_examples {

/**
 * Demonstrates drawing HUD-style geometry and text that is fixed to the window
 * and not the 3D scene.
 */
class HudRenderer : public sv::Renderer {
  Q_OBJECT

 public:
    HudRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void ShutdownGL() override;

    void RenderBegin() override;

 private:
    void UpdateGeometry();

    sv::MaterialResource::Ptr material_;
    sv::GeometryResource::Ptr geom_;
    bool geom_dirty_ = true;
    sv::GeometryData gdata_;
    sv::DrawNode* draw_node_;
    sv::CameraNode* hud_camera_;

    sv::TextBillboard::Ptr text_billboard_;

    QTime timer_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_EXAMPLES_HUD_RENDERER_HPP__
