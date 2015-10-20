#ifndef SCENEVIEW_GROUP_NODE_HPP__
#define SCENEVIEW_GROUP_NODE_HPP__

#include <memory>
#include <vector>

#include <sceneview/camera_node.hpp>
#include <sceneview/scene_node.hpp>

namespace sceneview {

class Scene;

class GroupNode : public SceneNode {
 public:
    virtual ~GroupNode() {}

    SceneNodeType NodeType() const override { return SceneNodeType::kGroupNode; }

    AxisAlignedBox BoundingBox(
        const QMatrix4x4& lhs_transform = QMatrix4x4()) override;

    const std::vector<SceneNode*>& Children() { return children_; }

 private:
    friend class Scene;

    GroupNode(const QString& name);

    SceneNode* AddChild(SceneNode* child);

    void CopyAsChildren(Scene* scene, GroupNode* root);

    void RemoveChild(SceneNode* child);

    std::vector<SceneNode*> children_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_GROUP_NODE_HPP__
