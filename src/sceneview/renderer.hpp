#ifndef SCENEVIEW_RENDERER_HPP__
#define SCENEVIEW_RENDERER_HPP__

#include <string>

#include <QObject>
#include <QVariant>

#include <sceneview/scene.hpp>
#include <sceneview/resource_manager.hpp>

namespace sceneview {

class Viewport;

class Renderer : public QObject {
  Q_OBJECT

 public:
    Renderer(const QString& name, QObject* parent = 0);

    Renderer(const Renderer&) = delete;

    Renderer& operator=(const Renderer&) = delete;

    const QString& Name() const { return name_; }

    Viewport* GetViewport() { return viewport_; }

    Scene::Ptr GetScene();

    ResourceManager::Ptr GetResources();

    /**
     * Retrieve the group node assigned to this renderer.
     *
     * The visibility of the base node is automatically toggled when
     * the renderer is enabled or disabled. Thus, any nodes that the renderer
     * creates that have the base node as an ancestor have their visibility
     * automatically managed by the base node.
     */
    GroupNode* GetBaseNode();

    /**
     * Override to acquire OpenGL resources required by the Renderer.
     *
     * At the time this method is invoked, the following are true:
     * - The OpenGL context is active.
     * - The Renderer has a base node.
     */
    virtual void InitializeGL() {}

    /**
     * Called at the start of rendering, just before the scene is rendered.
     */
    virtual void RenderBegin() {}

    /**
     * Called at the end of rendering, just after the scene has finished rendering.
     */
    virtual void RenderEnd() {}

    /**
     * Override this to release any OpenGL resources acquired by the renderer.
     */
    virtual void ShutdownGL() {}

    virtual QWidget* GetWidget() { return nullptr; }

    bool Enabled() const { return enabled_; }

    virtual QVariant SaveState() { return QVariant(); }

    virtual void LoadState(const QVariant& val) {}

 public slots:
    void SetEnabled(bool enabled);

 protected:
    /**
     *
     */
    virtual void OnEnableChanged(bool enabled) {}

 private:
    friend class Viewport;

    void SetViewport(Viewport* viewport);

    void SetBaseNode(GroupNode* node);

    QString name_;

    Viewport* viewport_;

    GroupNode* base_node_;

    bool enabled_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_RENDERER_HPP__
