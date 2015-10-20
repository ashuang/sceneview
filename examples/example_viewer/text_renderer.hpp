#ifndef SCENEVIEW_EXAMPLES_TEXT_RENDERER_HPP__
#define SCENEVIEW_EXAMPLES_TEXT_RENDERER_HPP__

#include <QTime>

#include <sceneview/renderer.hpp>
#include <sceneview/text_billboard.hpp>

namespace vis_examples {

class TextRenderer : public sceneview::Renderer {
  Q_OBJECT

 public:
    TextRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void ShutdownGL() override;

    void RenderBegin() override;

 private:
    void UpdateText();

    sceneview::TextBillboard::Ptr text_billboard_;

    QTime timer_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_EXAMPLES_TEXT_RENDERER_HPP__
