// Copyright [2016] Albert Huang

#ifndef SCENEVIEW_DRAW_GROUP_HPP__
#define SCENEVIEW_DRAW_GROUP_HPP__

#include <unordered_set>

#include <QString>

namespace sv {

class CameraNode;
class Scene;
class DrawNode;

enum class NodeOrdering {
  /**
   * Deprecated. Do not use.
   */
  kNone = 0,
  /**
   * Nodes are sorted by values returned by SceneNode::DrawOrder()
   */
  kNodeDrawOrder = 0,
  /**
   * Nodes are first sorted by values returned by SceneNode::DrawOrder().
   * Nodes with identical SceneNode::DrawOrder() values are sorted by depth
   * (nodes in back are drawn first).
   */
  kBackToFront = 1,
  /**
   * Nodes are first sorted by values returned by SceneNode::DrawOrder().
   * Nodes with identical SceneNode::DrawOrder() values are sorted by depth
   * (nodes in back are drawn first).
   */
  kFrontToBack = 2
};

class DrawGroup {
  public:
    const QString& Name() const { return name_; }

    int Order() const { return order_; }

    const std::unordered_set<DrawNode*>& DrawNodes() const { return nodes_; }

    /**
     * Sets the sorting method for drawing nodes in this draw group.
     */
    void SetNodeOrdering(NodeOrdering ordering) { node_ordering_ = ordering; }

    NodeOrdering GetNodeOrdering() const { return node_ordering_; }

    void SetFrustumCulling(bool value) { frustum_culling_ = value; }

    bool GetFrustumCulling() const { return frustum_culling_; }

    void SetCamera(CameraNode* camera) { camera_ = camera; }

    CameraNode* GetCamera() { return camera_; }

  private:
    friend class Scene;

    DrawGroup(const QString& name, int order);

    void AddNode(DrawNode* node);

    void RemoveNode(DrawNode* node);

    const QString name_;

    const int order_;

    NodeOrdering node_ordering_ = NodeOrdering::kBackToFront;

    bool frustum_culling_ = true;

    CameraNode* camera_ = nullptr;

    std::unordered_set<DrawNode*> nodes_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_GROUP_HPP__
