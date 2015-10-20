#ifndef SCENEVIEW_MESH_NODE_HPP__
#define SCENEVIEW_MESH_NODE_HPP__

#include <memory>
#include <utility>
#include <vector>

#include <sceneview/scene_node.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>

namespace sceneview {

/**
 * Renderable mesh with one or more geometry/material pairs.
 */
class MeshNode : public SceneNode {
 public:
 public:
    virtual ~MeshNode() {}

    SceneNodeType NodeType() const override { return SceneNodeType::kMeshNode; }

    void Add(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr material);

    void Add(GeometryMaterialPair component);

    const std::vector<GeometryMaterialPair>& Components() const;

    /**
     * Retrieve the bounding box of the individual components, transformed by
     * this node's transform as well as any transform passed in.
     *
     * @param lhs_transform the transform to apply on the left
     *
     * If this node's transform is a matrix M, and lhs_transform is a matrix L,
     * then this function calculates
     *
     * @code
     * bounding_box(L * M * vertices_of(GeometryBoundingBox()));
     * @endcode
     */
    virtual AxisAlignedBox BoundingBox(
        const QMatrix4x4& lhs_transform = QMatrix4x4()) override;

    /**
     * Retrieve the bounding box of the individual components, untransformed.
     */
    AxisAlignedBox GeometryBoundingBox();

 private:
    friend class Scene;

    MeshNode(const QString& name);

    std::vector<GeometryMaterialPair> components_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_MESH_NODE_HPP__
