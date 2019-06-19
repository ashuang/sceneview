#ifndef SCENEVIEW_STENCIL_RENDERER_HPP__
#define SCENEVIEW_STENCIL_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

namespace vis_examples {

/**
 * Demonstrates using the OpenGL stencil buffer.
 *
 * See: https://www.khronos.org/opengl/wiki/Stencil_Test
 */
class StencilRenderer : public sv::Renderer {
  Q_OBJECT

 public:
    StencilRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

 private:
    std::vector<sv::DrawNode*> shapes_;

    sv::MaterialResource::Ptr material_a_;
    sv::MaterialResource::Ptr material_b_;
    sv::GeometryResource::Ptr geom_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_STENCIL_RENDERER_HPP__
