// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_GROUP_NODE_HPP__
#define SCENEVIEW_GROUP_NODE_HPP__

#include <vector>

#include <sceneview/scene_node.hpp>

namespace sv {

class Scene;

class GroupNode : public SceneNode {
  public:
    virtual ~GroupNode() {}

    SceneNodeType NodeType() const override {
      return SceneNodeType::kGroupNode; }

    AxisAlignedBox BoundingBox(
        const QMatrix4x4& lhs_transform = QMatrix4x4()) override;

    const std::vector<SceneNode*>& Children() { return children_; }

  private:
    friend class Scene;

    explicit GroupNode(const QString& name);

    SceneNode* AddChild(SceneNode* child);

    void CopyAsChildren(Scene* scene, GroupNode* root);

    void RemoveChild(SceneNode* child);

    std::vector<SceneNode*> children_;
};

}  // namespace sv

#endif  // SCENEVIEW_GROUP_NODE_HPP__
