// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_GRID_RENDERER_HPP__
#define SCENEVIEW_GRID_RENDERER_HPP__

#include <sceneview/renderer.hpp>
#include <sceneview/material_resource.hpp>

namespace sv {

/**
 * A stock renderer provided as a basic utility and an example.
 *
 * Draws a grid in the scene.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/grid_renderer.hpp
 */
class GridRenderer : public Renderer {
  Q_OBJECT

  public:
    explicit GridRenderer(const QString& name, QObject* parent = 0);

    virtual ~GridRenderer();

    void InitializeGL() override;

    void RenderBegin() override;

  private:
    void UpdateGeometry();

    struct Priv;

    Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_GRID_RENDERER_HPP__
