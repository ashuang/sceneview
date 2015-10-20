// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_RENDERER_HPP__
#define SCENEVIEW_RENDERER_HPP__

#include <string>

#include <QObject>
#include <QVariant>

#include <sceneview/scene.hpp>
#include <sceneview/resource_manager.hpp>

namespace sv {

class Viewport;

/**
 * Subclass this to add content in the scene and draw things.
 *
 * The primary way to add content into a scene and draw using Sceneview is to
 * create subclasses of Renderer and add them to a Viewport.
 *
 * ## Drawing with a %Renderer
 * There are two ways to draw using a subclass of Renderer:
 * 1. Using the Sceneview rendering engine and inserting nodes into the scene
 *    graph.
 * 2. Directly calling OpenGL drawing functions.
 *
 * ### Using the Sceneview rendering engine
 *
 * The Renderer class provides a few mechanisms to support using the Sceneview
 * rendering engine.
 *
 * - Every Renderer is automatically assigned a GroupNode, referred to as the
 *   Renderer's base node. You can access this by calling GetBaseNode().
 * - When the renderer is enabled / disabled, the base node visibility is
 *   automatically toggled by the Viewer.
 * - You can access the scene graph and resource manager using GetScene() and
 *   GetResources().
 *
 * ### Directly calling OpenGL drawing functions.
 * If you want to bypass the Sceneview rendering engine, you can issue direct
 * calls to OpenGL by overriding RenderBegin() and RenderEnd().
 *
 * Once each render cycle, the following happens:
 * 1. Sceneview sets up the OpenGL fixed function pipeline (initializes the
 *    GL_PROJECTION, GL_MODELVIEW matrices, sets up the OpenGL lights according
 *    to the scene graph, etc)
 * 2. RenderBegin() is called on every enabled renderer.
 * 3. The scene graph is rendererd using the Sceneview rendering engine.
 * 4. The OpenGL fixed function pipline is setup again, same as in step 1.
 * 5. RenderEnd() is called on every enabled renderer.
 *
 * ## OpenGL context management
 *
 * When a renderer is first created, there is no guarantee that the OpenGL
 * context has been created or activated. The first time this is guaranteed is
 * when InitializeGL() is called.
 *
 * Similarly, the OpenGL context may be gone by the time the Renderer
 * destructor is called. Instead of releasing OpenGL resources in the object
 * destructor, do so by overriding ShutdownGL().
 *
 * ## %Renderer state
 * If you want to save the state of your renderer across sessions, you can
 * override SaveState() and LoadState() to return a QVariant that will be
 * used whenever the Viewer itself is saving / loading state.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/renderer.hpp
 */
class Renderer : public QObject {
  Q_OBJECT

  public:
    /**
     * Construct a new renderer with the specified name.
     */
    explicit Renderer(const QString& name, QObject* parent = 0);

    Renderer(const Renderer&) = delete;

    Renderer& operator=(const Renderer&) = delete;

    /**
     * Retrieve the renderer name.
     */
    const QString& Name() const { return name_; }

    /**
     * Retrieve the viewport that manages this renderer.
     */
    Viewport* GetViewport() { return viewport_; }

    /**
     * Retrieve the Scene graph used by the Sceneview rendering engine.
     */
    Scene::Ptr GetScene();

    /**
     * Retrieve the ResourceManager for the scene.
     */
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
     *
     * It is guaranteed that this method will be called exactly once, and
     * before any call to RenderBegin().
     */
    virtual void InitializeGL() {}

    /**
     * Called at the start of rendering, just before the scene is rendered.
     *
     * When this method is called, the following are true:
     * - No shader programs are active (i.e., glUseProgram(0) has been called)
     * - The projection matrix (GL_PROJECTION) is setup according to the active
     *   camera.
     * - The modelview matrix stack has a single matrix on it that correspnods to
     *   the camera view matrix.
     * - The GL_MODELVIEW matrix stack is active.
     * - OpenGL lights have been configured according to the lights in the
     *   scene graph.
     *
     * In other words, the matrix stack is setup such that you can render in
     * "world" coordinates.
     *
     * You can issue legacy fixed-function OpenGL commands (e.g., glColor3f(),
     * glBegin(), glVertex3f(), etc. calls)
     *
     * You can also use your own vertex and fragment shaders.
     */
    virtual void RenderBegin() {}

    /**
     * Called at the end of rendering, just after the scene has finished rendering.
     *
     * This method has the same guarantees as RenderBegin().
     */
    virtual void RenderEnd() {}

    /**
     * Override this to release any OpenGL resources acquired by the renderer.
     *
     * This method is called when the rendering engine is shutting down, just
     * before the OpenGL context is destroyed.
     */
    virtual void ShutdownGL() {}

    /**
     * Override this to provide a custom UI for your renderer.
     */
    virtual QWidget* GetWidget() { return nullptr; }

    /**
     * @return true if the renderer is enabled, false if not.
     *
     * If the renderer is disabled, then RenderBegin() and RenderEnd() are not
     * called during the render cycle. Additinally, the renderer's base node
     * visibility is automatically set to match wheter or not the renderer is
     * enabled.
     */
    bool Enabled() const { return enabled_; }

    /**
     * Called by the viewport to save the renderer state. If your renderer has
     * any adjustable settings that you want to persist, then save them into
     * the returned QVariant.
     */
    virtual QVariant SaveState() { return QVariant(); }

    /**
     * Called by the viewport to restore the renderer state. If your renderer
     * has any adjustable settings that you want to persist across sessions,
     * then load them from the passed in QVariant here.
     */
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

}  // namespace sv

#endif  // SCENEVIEW_RENDERER_HPP__
