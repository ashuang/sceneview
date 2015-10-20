#ifndef SCENEVIEW_EXAMPLES_FIXED_FUNCTION_RENDERER_HPP__
#define SCENEVIEW_EXAMPLES_FIXED_FUNCTION_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

namespace vis_examples {

/**
 * Demonstrates rendering using the OpenGL fixed function pipeline.
 */
class FixedFunctionRenderer : public sceneview::Renderer {
  Q_OBJECT

 public:
    FixedFunctionRenderer(const QString& name, QObject* parent = 0);

    void RenderBegin() override;

    void RenderEnd() override;

 private:
    QTime counter_;
    double angle_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_EXAMPLES_FIXED_FUNCTION_RENDERER_HPP__
