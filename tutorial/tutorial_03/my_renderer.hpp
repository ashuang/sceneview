#ifndef SCENEVIEW_TUTORIAL_MY_RENDERER_HPP__
#define SCENEVIEW_TUTORIAL_MY_RENDERER_HPP__

#include <sceneview/sceneview.hpp>

class MyRenderer : public sv::Renderer {
  Q_OBJECT

  public:
    MyRenderer(const QString& name, QWidget* parent = nullptr);

    void InitializeGL() override;
};

#endif  // SCENEVIEW_TUTORIAL_MY_RENDERER_HPP__
