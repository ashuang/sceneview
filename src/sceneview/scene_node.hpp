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
  kDrawNode
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
     * Retrieve the translation component of the node to parent transform.
     */
    const QVector3D& Translation() const { return translation_; }

    /**
     * Retrieve the rotation component of the node to parent transform.
     */
    const QQuaternion& Rotation() const { return rotation_; }

    /**
     * Retrieve the scale component of the node to parent transform.
     */
    const QVector3D& Scale() const { return scale_; }

    /**
     * Retrieve the transform from node coordinates to world coordinates.
     *
     * The world transform is obtained by chaining the node to parent
     * transforms of all nodes from this node until the root.
     */
    const QMatrix4x4& WorldTransform();

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
     * Sets the rotation component of the node to parent transform.
     */
    virtual void SetRotation(const QQuaternion& quat);

    /**
     * Sets the scale component of the node to parent transform.
     */
    virtual void SetScale(const QVector3D& vec);

    /**
     * Sets the scale component of the node to parent transform.
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
     * Retrieve the parent of this node.
     */
    GroupNode* ParentNode() { return parent_node_; }

    /**
     * Sets the node's parent. This is typically automatically handled by the
     * Scene graph.
     */
    void SetParentNode(GroupNode* parent);

    /**
     * Sets the selection mask for this node.
     *
     * The default selection mask for all nodes is 0.
     *
     * For information on how to use this, see SelectionQuery.
     */
    void SetSelectionMask(int64_t mask) { selection_mask_ = mask; }

    /**
     * Retrieve the selection mask for this node.
     */
    int64_t GetSelectionMask() const { return selection_mask_; }

    /**
     * Retrieve the world-space bounding box of the node and all of its
     * children (if applicable).
     *
     * Used internally in view frustum culling and selection queries.
     */
    virtual const AxisAlignedBox& WorldBoundingBox() = 0;

    /**
     * Set the draw order of this node within the draw group. Use this to force
     * certain nodes to draw before or after other nodes within the draw group.
     *
     * Nodes with a lower order are drawn first.
     */
    void SetDrawOrder(int order) { draw_order_ = order; }

    int DrawOrder() const { return draw_order_; }

  protected:
    /**
     * Constructs a scene node with an identity transform.
     */
    explicit SceneNode(const QString& name);

    /**
     * Internal method, used to enable lazy matrix computations.
     * Called when the node's transform changes (e.g., via SetTranslation,
     * SetRotation, or SetScale) or when a parent's transform changes. Also
     * triggers a call to BoundingBoxChanged()
     */
    virtual void TransformChanged();

    /**
     * Internal method, used to enable lazy bounding box computations.
     * Called when the node's bounding box changes, or when a child's bounding
     * box changes.
     */
    virtual void BoundingBoxChanged();

  private:
    friend class GroupNode;

    const QString node_name_;

    QVector3D translation_;
    QQuaternion rotation_;
    QVector3D scale_{1, 1, 1};

    QMatrix4x4 to_world_;
    bool to_world_dirty_ = true;

    GroupNode* parent_node_ = nullptr;

    bool visible_ = true;
    int64_t selection_mask_ = 0;

    int draw_order_ = 0;
};

}  // namespace sv

#endif  // SCENEVIEW_SCENE_NODE_HPP__
