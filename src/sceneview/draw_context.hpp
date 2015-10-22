// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_DRAW_CONTEXT_HPP__
#define SCENEVIEW_DRAW_CONTEXT_HPP__

#include <sceneview/drawable.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

class QOpenGLShaderProgram;

namespace sv {

class AxisAlignedBox;
class CameraNode;
class MeshNode;

class DrawContext {
  public:
    DrawContext(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene);

    void Draw(CameraNode* camera);

  private:
    void DrawMesh(MeshNode* mesh);

    void ActivateMaterial();

    void DrawGeometry();

    void DrawBoundingBox(const AxisAlignedBox& box);

    ResourceManager::Ptr resources_;

    Scene::Ptr scene_;

    // Rendering variables
    CameraNode* cur_camera_;

    MaterialResource::Ptr material_;
    GeometryResource::Ptr geometry_;
    ShaderResource::Ptr shader_;
    QOpenGLShaderProgram* program_;
    QMatrix4x4 model_mat_;

    // For debugging
    MeshNode* bounding_box_mesh_;
    bool draw_bounding_boxes_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_CONTEXT_HPP__
