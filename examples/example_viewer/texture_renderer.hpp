#ifndef SCENEVIEW_EXAMPLES_TEXTURE_RENDERER_HPP__
#define SCENEVIEW_EXAMPLES_TEXTURE_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

class QOpenGLTexture;

namespace vis_examples {

/**
 * Demonstrates rendering texture-mapped geometry.
 */
class TextureRenderer : public sv::Renderer {
  Q_OBJECT

 public:
    TextureRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void RenderBegin() override;

 private:
    void MakeTexture();

    QTime start_time_;

    const int tex_width_ = 400;
    const int tex_height_ = 400;
    std::shared_ptr<QOpenGLTexture> texture_;

    sv::MaterialResource::Ptr material_;
    sv::GeometryResource::Ptr geom_;
    sv::GeometryData gdata_;
    sv::DrawNode* draw_node_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_EXAMPLES_TEXTURE_RENDERER_HPP__
