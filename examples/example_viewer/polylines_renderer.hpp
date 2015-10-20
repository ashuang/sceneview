#ifndef SCENEVIEW_EXAMPLES_POLYLINES_RENDERER_HPP__
#define SCENEVIEW_EXAMPLES_POLYLINES_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

namespace vis_examples {

class PolylinesRenderer : public sceneview::Renderer {
  Q_OBJECT

 public:
    PolylinesRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void RenderBegin() override;

 private:
    void UpdateGeometry();

    sceneview::MaterialResource::Ptr material_;
    sceneview::GeometryResource::Ptr geom_;
    sceneview::MeshNode* mesh_;

    QTime start_time_;
    double angle_;

    std::unique_ptr<sceneview::ParamWidget> widget_;
    sceneview::GeometryData gdata_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_EXAMPLES_POLYLINES_RENDERER_HPP__
