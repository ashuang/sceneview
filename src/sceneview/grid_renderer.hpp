#ifndef SCENEVIEW_GRID_RENDERER_HPP__
#define SCENEVIEW_GRID_RENDERER_HPP__

#include <sceneview/renderer.hpp>
#include <sceneview/material_resource.hpp>

namespace sceneview {

class GridRenderer : public Renderer {
  Q_OBJECT

 public:
    GridRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void RenderBegin() override;

 private:
    void UpdateGeometry();

    MaterialResource::Ptr base_material_;
    MaterialResource::Ptr grid_material_;
    MaterialResource::Ptr depth_write_material_;

    GeometryResource::Ptr grid_geom_;
    GeometryResource::Ptr base_geom_;
    MeshNode* mesh_;

    int grid_size_;

    GeometryData gdata_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_GRID_RENDERER_HPP__
