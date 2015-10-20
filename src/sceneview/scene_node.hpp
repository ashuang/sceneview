// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SCENE_NODE_HPP__
#define SCENEVIEW_SCENE_NODE_HPP__

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

#include <sceneview/axis_aligned_box.hpp>

namespace sv {

enum class SceneNodeType {
  kGroupNode,
  kCameraNode,
  kLightNode,
  kMeshNode
};

class GroupNode;

class SceneNode {
  public:
    SceneNode(const SceneNode&) = delete;

    SceneNode& operator=(const SceneNode&) = delete;

    virtual ~SceneNode() {}

    virtual SceneNodeType NodeType() const = 0;

    const QString Name() const { return node_name_; }

    const QVector3D& Translation() const { return translation_; }

    const QQuaternion& Rotation() const { return rotation_; }

    const QVector3D& Scale() const { return scale_; }

    const QMatrix4x4& GetTransform();

    bool Visible() const { return visible_; }

    virtual void SetTranslation(const QVector3D& vec);

    void SetTranslation(double x, double y, double z) {
      SetTranslation(QVector3D(x, y, z));
    }

    virtual void SetRotation(const QQuaternion& quat);

    virtual void SetScale(const QVector3D& vec);

    void SetScale(double x, double y, double z) {
      SetScale(QVector3D(x, y, z));
    }

    virtual void SetVisible(bool visible);

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
        const QMatrix4x4& lhs_transform = QMatrix4x4());

    GroupNode* ParentNode() { return parent_node_; }

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
