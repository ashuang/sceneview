#ifndef SCENEVIEW_DRAW_SCENE_HPP__
#define SCENEVIEW_DRAW_SCENE_HPP__

#include <sceneview/axis_aligned_box.hpp>
#include <sceneview/camera_node.hpp>
#include <sceneview/mesh_node.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

namespace sceneview {

class DrawScene {
  public:
    DrawScene(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene);

    void Draw(CameraNode* camera);

  private:
    void DrawMesh(MeshNode* mesh);

    void DrawMeshCmoponent(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material,
        const QMatrix4x4& mesh_to_world);

    void DrawBoundingBox(const AxisAlignedBox& box);

    ResourceManager::Ptr resources_;

    Scene::Ptr scene_;

    // Rendering variables
    CameraNode* cur_camera_;

    // For debugging
    MeshNode* bounding_box_mesh_;
    bool draw_bounding_boxes_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_DRAW_SCENE_HPP__
