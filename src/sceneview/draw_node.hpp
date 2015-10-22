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

/**
 * Scene node that contains a list of drawable objects.
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/draw_node.hpp
 */
class DrawNode : public SceneNode {
  public:
    virtual ~DrawNode() {}

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

    /**
     * Retrieve the bounding box of the individual components, transformed by
     * this node's transform as well as any transform passed in.
     *
     * @param lhs_transform the transform to apply on the left
     *
     * If this node's transform is a matrix M,
     * then this function calculates
     *
     * @code
     * bounding_box(lhs * M * vertices_of(GeometryBoundingBox()));
     * @endcode
     */
    AxisAlignedBox BoundingBox(const QMatrix4x4& lhs = QMatrix4x4());

    /**
     * Retrieve the bounding box of the individual components, untransformed.
     */
    AxisAlignedBox GeometryBoundingBox();

  private:
    friend class Scene;

    explicit DrawNode(const QString& name);

    std::vector<Drawable::Ptr> drawables_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_NODE_HPP__
