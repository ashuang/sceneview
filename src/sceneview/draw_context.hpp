// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_DRAW_CONTEXT_HPP__
#define SCENEVIEW_DRAW_CONTEXT_HPP__

#include <vector>

#include <QColor>

#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

namespace sv {

class AxisAlignedBox;
class DrawGroup;
class DrawNode;
class Renderer;

class DrawContext {
  public:
    DrawContext(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene);

    virtual ~DrawContext();

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

    class Priv;

    Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_CONTEXT_HPP__
