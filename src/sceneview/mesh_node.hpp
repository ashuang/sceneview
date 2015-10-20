// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_MESH_NODE_HPP__
#define SCENEVIEW_MESH_NODE_HPP__

#include <memory>
#include <utility>
#include <vector>

#include <sceneview/scene_node.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>

namespace sv {

/**
 * Renderable mesh with one or more geometry/material pairs.
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/mesh_node.hpp
 */
class MeshNode : public SceneNode {
  public:
    virtual ~MeshNode() {}

    SceneNodeType NodeType() const override { return SceneNodeType::kMeshNode; }

    /**
     * Attaches a geometry resource and a material resource to the mesh so
     * that they are drawn as part of the mesh.
     *
     * A mesh can have multiple geometry/material pairs, and they are always
     * drawn in the order that they are added to the mesh, relative to each
     * other.
     */
    void Add(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material);

    /**
     * Attaches a geometry resource and a material resource to the mesh so
     * that they are drawn as part of the mesh.
     *
     * A mesh can have multiple geometry/material pairs, and they are always
     * drawn in the order that they are added to the mesh, relative to each
     * other.
     */
    void Add(GeometryMaterialPair component);

    /**
     * Retrieve a list of the geometry/material pairs attached to the mesh.
     */
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
    AxisAlignedBox BoundingBox(
        const QMatrix4x4& lhs_transform = QMatrix4x4()) override;

    /**
     * Retrieve the bounding box of the individual components, untransformed.
     */
    AxisAlignedBox GeometryBoundingBox();

  private:
    friend class Scene;

    explicit MeshNode(const QString& name);

    std::vector<GeometryMaterialPair> components_;
};

}  // namespace sv

#endif  // SCENEVIEW_MESH_NODE_HPP__
