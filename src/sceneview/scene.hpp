// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SCENE_HPP__
#define SCENEVIEW_SCENE_HPP__

#include <map>
#include <memory>
#include <vector>

#include <QString>

#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>

namespace sv {

class CameraNode;
class GroupNode;
class LightNode;
class DrawNode;
class SceneNode;
class DrawGroup;

/**
 * A scene graph.
 *
 * Scene objects are typically used to represent either a scene to be drawn, or
 * a 3D model loaded into memory.
 *
 * Scene objects cannot be directly instantiated. Instead, use ResourceManager.
 *
 * @ingroup sv_resources
 * @ingroup sv_scenegraph
 * @headerfile sceneview/scene.hpp
 */
class Scene {
  public:
    typedef std::shared_ptr<Scene> Ptr;

    static const QString kAutoName;

    /**
     * The order assigned to the default render group.
     */
    static constexpr int kDefaultDrawGroupOrder = 10;

    /**
     * Name of the default render group. This is equal to "default".
     */
    static const QString kDefaultDrawGroupName;

  public:
    ~Scene();

    const QString& Name() const { return scene_name_; }

    /**
     * Retrieve the root node.
     *
     * The root node of a scene is always a group node.
     */
    GroupNode* Root() { return root_node_; }

    bool ContainsNode(SceneNode* node) const;

    /**
     * Create an empty group node.
     */
    GroupNode* MakeGroup(GroupNode* parent,
        const QString& name = kAutoName);

    /**
     * Create a group node populated as a copy of the specified scene graph.
     *
     * @param parent the parent node for the new group node
     * @param scene the scene to copy into the new node. Cannot be this scene.
     */
    GroupNode* MakeGroupFromScene(GroupNode* parent,
        Scene::Ptr scene,
        const QString& name = kAutoName);

    /**
     * Create a camera. The returned camera is owned by this object.
     *
     * @return the newly created camera.
     */
    CameraNode* MakeCamera(GroupNode* parent,
        const QString& name = kAutoName);

    /**
     * Create a light. The returned light is owned by this object.
     *
     * @return a newly created LightNode.
     */
    LightNode* MakeLight(GroupNode* parent,
        const QString& name = kAutoName);

    /**
     * Create an empty draw node. The returned node is owned by this object.
     *
     * To attach drawables to the node, use DrawNode::Add().
     *
     * @return a newly created DrawNode.
     */
    DrawNode* MakeDrawNode(GroupNode* parent,
        const QString& name = kAutoName);

    /**
     * Create a draw node with a single drawable.
     *
     * The returned node is owned by this object.
     *
     * You can attach additional drawables to the draw node by calling
     * DrawNode::Add().
     *
     * @return a newly created DrawNode.
     */
    DrawNode* MakeDrawNode(GroupNode* parent,
        const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material,
        const QString& name = kAutoName);

    /**
     * Create a draw group.
     *
     * The returned DrawGroup is owned by this object.
     *
     * @return a newly created draw group.
     */
    DrawGroup* MakeDrawGroup(int ordering,
        const QString& name = kAutoName);

    /**
     * Sets the draw group that a draw node belongs to.
     */
    void SetDrawGroup(DrawNode* draw_node, DrawGroup* draw_group);

    /**
     * Convenience method to call SetDrawGroup() on all DrawNode descendants of
     * the specified group.
     */
    void SetDrawGroup(GroupNode* node, DrawGroup* draw_group);

    /**
     * Destroys a node and all of its children.
     */
    void DestroyNode(SceneNode* node);

    /**
     * Destroys a draw group.
     *
     * Constraints:
     * - The default draw group cannot be destroyed.
     * - The draw group must be empty (i.e., no nodes assigned to the draw
     *   group).
     */
    void DestroyDrawGroup(DrawGroup* draw_group);

    /**
     * Retrieve a list of all lights in the scene.
     *
     * Don't modify the returned vector.
     */
    std::vector<LightNode*>& Lights() { return lights_; }

    /**
     * Retrieve the draw group with the specified name.
     *
     * @throw std::invalid_argument if @c name does not correspond to a draw
     * group in the scene.
     */
    DrawGroup* GetDrawGroup(const QString& name);

    DrawGroup* GetDefaultDrawGroup() { return default_draw_group_; }

    void PrintStats();

  private:
    friend class ResourceManager;

    explicit Scene(const QString& name);

    QString AutogenerateName();

    QString PickName(const QString& name);

    QString scene_name_;

    GroupNode* root_node_;

    int name_counter_;

    DrawGroup* default_draw_group_;

    std::vector<LightNode*> lights_;
    std::vector<CameraNode*> cameras_;
    std::vector<DrawGroup*> draw_groups_;
    std::map<QString, SceneNode*> nodes_;
};

}  // namespace sv

#endif  // SCENEVIEW_SCENE_HPP__
