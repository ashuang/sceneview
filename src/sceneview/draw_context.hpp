// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_DRAW_CONTEXT_HPP__
#define SCENEVIEW_DRAW_CONTEXT_HPP__

#include <vector>

#include <QColor>

#include <sceneview/drawable.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

class QOpenGLShaderProgram;

namespace sv {

class AxisAlignedBox;
class CameraNode;
class DrawGroup;
class DrawNode;
class Renderer;
class Plane;

class DrawContext {
  public:
    DrawContext(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene);

    void Draw(int viewport_width,
        int viewport_height,
        std::vector<Renderer*>* prenderers);

    /**
     * Value passed to glClearColor() at the beginning of each render cycle.
     */
    void SetClearColor(const QColor& color);

    void SetDrawGroups(const std::vector<DrawGroup*>& groups);

  private:
    void PrepareFixedFunctionPipeline();

    void DrawDrawGroup(DrawGroup* dgroup);

    void DrawDrawNode(DrawNode* node);

    void ActivateMaterial();

    void DrawGeometry();

    void DrawBoundingBox(const AxisAlignedBox& box);

    ResourceManager::Ptr resources_;

    Scene::Ptr scene_;

    QColor clear_color_;

    // Rendering variables
    int viewport_width_ = 0;
    int viewport_height_ = 0;
    CameraNode* cur_camera_ = nullptr;

    MaterialResource::Ptr material_;
    GeometryResource::Ptr geometry_;
    ShaderResource::Ptr shader_;
    QOpenGLShaderProgram* program_;
    QMatrix4x4 model_mat_;

    std::vector<DrawGroup*> draw_groups_;

    bool gl_two_sided_;
    bool gl_depth_test_;
    GLenum gl_depth_func_;
    bool gl_depth_write_;
    bool gl_color_write_;
    float gl_point_size_;
    float gl_line_width_;
    bool gl_blend_;
    GLenum gl_sfactor_;
    GLenum gl_dfactor_;

    // For debugging
    DrawNode* bounding_box_node_;
    bool draw_bounding_boxes_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_CONTEXT_HPP__
