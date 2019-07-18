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
    virtual ~DrawGroup();

    const QString& Name() const;

    int Order() const;

    const std::unordered_set<DrawNode*>& DrawNodes() const;

    /**
     * Sets the sorting method for drawing nodes in this draw group.
     */
    void SetNodeOrdering(NodeOrdering ordering);

    NodeOrdering GetNodeOrdering() const;

    void SetFrustumCulling(bool value);

    bool GetFrustumCulling() const;

    void SetCamera(CameraNode* camera);

    CameraNode* GetCamera();

  private:
    friend class Scene;

    DrawGroup(const QString& name, int order);

    void AddNode(DrawNode* node);

    void RemoveNode(DrawNode* node);

    class Priv;

    Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAW_GROUP_HPP__
