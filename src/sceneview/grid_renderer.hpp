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

    void InitializeGL() override;

    void RenderBegin() override;

  private:
    void UpdateGeometry();

    MaterialResource::Ptr base_material_;
    MaterialResource::Ptr grid_material_;
    MaterialResource::Ptr depth_write_material_;

    GeometryResource::Ptr grid_geom_;
    GeometryResource::Ptr base_geom_;
    DrawNode* draw_node_;

    int grid_size_;

    GeometryData gdata_;
};

}  // namespace sv

#endif  // SCENEVIEW_GRID_RENDERER_HPP__
