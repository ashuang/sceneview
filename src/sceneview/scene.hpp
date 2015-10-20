#ifndef SCENEVIEW_SCENE_HPP__
#define SCENEVIEW_SCENE_HPP__

#include <map>
#include <memory>

#include <QString>

namespace sceneview {

class CameraNode;
class GroupNode;
class LightNode;
class MeshNode;
class SceneNode;

/**
 * A scene graph.
 */
class Scene {
  public:
    typedef std::shared_ptr<Scene> Ptr;

    static const QString kAutoName;

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
     * Create a camera.
     *
     * @return the newly created camera.
     */
    CameraNode* MakeCamera(GroupNode* parent,
        const QString& name = kAutoName);

    LightNode* MakeLight(GroupNode* parent,
        const QString& name = kAutoName);

    MeshNode* MakeMesh(GroupNode* parent,
        const QString& name = kAutoName);

    /**
     * Destroys a node and all of its children.
     */
    void DestroyNode(SceneNode* node);

    /**
     * Retrieve a list of all lights in the scene.
     *
     * Don't modify the returned vector.
     */
    std::vector<LightNode*>& Lights() { return lights_; }

    /**
     * Retrieve a list of all meshes in the scene
     *
     * Don't modify the returned vector.
     */
    std::vector<MeshNode*>& Meshes() { return meshes_; }

    void PrintStats();

  private:
    friend class ResourceManager;

    Scene(const QString& name);

    QString AutogenerateName();

    QString PickName(const QString& name);

    QString scene_name_;

    GroupNode* root_node_;

    int name_counter_;

    std::vector<LightNode*> lights_;
    std::vector<MeshNode*> meshes_;
    std::vector<CameraNode*> cameras_;
    std::map<QString, SceneNode*> nodes_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_SCENE_HPP__
