// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SCENE_NODE_HPP__
#define SCENEVIEW_SCENE_NODE_HPP__

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

#include <sceneview/axis_aligned_box.hpp>

namespace sv {

/**
 * Specifies a scene node type.
 *
 * @ingroup sv_scenegraph
 */
enum class SceneNodeType {
  kGroupNode,
  kCameraNode,
  kLightNode,
  kMeshNode
};

class GroupNode;

/**
 * Pure virtual class that all scene graph nodes inherit.
 *
 * The SceneNode class maintains state common to all scene graph nodes:
 * - Node transform.
 *   - Represented as Translation * Rotation * Scale.
 * - Node name.
 * - Node visibility.
 * - Parent node.
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/scene_node.hpp
 */
class SceneNode {
  public:
    SceneNode(const SceneNode&) = delete;

    SceneNode& operator=(const SceneNode&) = delete;

    virtual ~SceneNode() {}

    /**
     * Retrieve the node type.
     */
    virtual SceneNodeType NodeType() const = 0;

    /**
     * Retrieve the node name.
     */
    const QString Name() const { return node_name_; }

    /**
     * Retrieve the translation component of the node transform.
     */
    const QVector3D& Translation() const { return translation_; }

    /**
     * Retrieve the rotation component of the node transform.
     */
    const QQuaternion& Rotation() const { return rotation_; }

    /**
     * Retrieve the scale component of the node transform.
     */
    const QVector3D& Scale() const { return scale_; }

    /**
     * Retrieve the node transform.
     *
     * Internally, the node transform is represented as the matrix:
     * M = T * R * S
     *
     * where T is a translation matrix, R is a rotation matrix, and S is a
     * scale matrix.
     */
    const QMatrix4x4& GetTransform();

    /**
     * Check if the node is visible or not.
     */
    bool Visible() const { return visible_; }

    /**
     * Sets the translation component of the node transform.
     */
    virtual void SetTranslation(const QVector3D& vec);

    /**
     * Sets the translation component of the node transform.
     */
    void SetTranslation(double x, double y, double z) {
      SetTranslation(QVector3D(x, y, z));
    }

    /**
     * Sets the rotation component of the node transform.
     */
    virtual void SetRotation(const QQuaternion& quat);

    /**
     * Sets the scale component of the node transform.
     */
    virtual void SetScale(const QVector3D& vec);

    /**
     * Sets the scale component of the node transform.
     */
    void SetScale(double x, double y, double z) {
      SetScale(QVector3D(x, y, z));
    }

    /**
     * Sets the node visibility.
     *
     * Invisible nodes are not drawn during a render cycle, along with their
     * children.
     */
    virtual void SetVisible(bool visible);

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
     * bounding_box(lhs_transform * M * vertices_of(GeometryBoundingBox()));
     * @endcode
     */
    virtual AxisAlignedBox BoundingBox(
        const QMatrix4x4& lhs_transform = QMatrix4x4());

    /**
     * Retrieve the parent of this node.
     */
    GroupNode* ParentNode() { return parent_node_; }

    /**
     * Sets the node's parent. This is typically automatically handled by the
     * Scene graph.
     */
    void SetParentNode(GroupNode* parent);

  protected:
    /**
     * Constructs a scene node with an identity transform.
     */
    explicit SceneNode(const QString& name);

  private:
    const QString node_name_;

    QVector3D translation_;
    QQuaternion rotation_;
    QVector3D scale_;

    QMatrix4x4 transform_;

    GroupNode* parent_node_;

    bool visible_;
};

}  // namespace sv

#endif  // SCENEVIEW_SCENE_NODE_HPP__
