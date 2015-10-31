// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_DRAW_NODE_HPP__
#define SCENEVIEW_DRAW_NODE_HPP__

#include <memory>
#include <utility>
#include <vector>

#include <sceneview/drawable.hpp>
#include <sceneview/scene_node.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>

namespace sv {

class Drawable;

/**
 * Scene node that contains a list of drawable objects.
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/draw_node.hpp
 */
class DrawNode : public SceneNode {
  public:
    virtual ~DrawNode();

    SceneNodeType NodeType() const override { return SceneNodeType::kDrawNode; }

    /**
     * Attaches a generic drawable item to the node.
     *
     * The node can have multiple drawable objects, and they are always drawn
     * in the order that they are added, relative to each other.
     */
    void Add(const Drawable::Ptr& drawable);

    /**
     * Convenience method to attach a drawable object from a geometry resource
     * and a material resource.
     *
     * This method is equivalent to:
     * @code
     *    draw_node->Add(Drawable::Create(geometry, material));
     * @endcode
     */
    void Add(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material);

    /**
     * Retrieve a list of the drawables attached to the node.
     */
    const std::vector<Drawable::Ptr>& Drawables() const;

    const AxisAlignedBox& WorldBoundingBox() override;

  protected:
    void BoundingBoxChanged() override;

  private:
    friend class Scene;

    friend class Drawable;

    explicit DrawNode(const QString& name);

    std::vector<Drawable::Ptr> drawables_;

    AxisAlignedBox bounding_box_;
    bool bounding_box_dirty_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_NODE_HPP__
