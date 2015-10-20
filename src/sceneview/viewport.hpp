#ifndef SCENEVIEW_VIEWPORT_HPP__
#define SCENEVIEW_VIEWPORT_HPP__

#include <vector>

#include <QOpenGLWidget>

#include <sceneview/camera_node.hpp>
#include <sceneview/draw_scene.hpp>
#include <sceneview/renderer.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>
#include <sceneview/input_handler.hpp>

namespace sceneview {

class RendererList {
  public:
};

class Viewport : public QOpenGLWidget {
  Q_OBJECT

 public:
    explicit Viewport(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene,
        QWidget* parent = nullptr);

    Viewport(const Viewport&) = delete;

    Viewport& operator=(const Viewport&) = delete;

    ~Viewport();

    void AddRenderer(Renderer* renderer);

    void AddInputHandler(InputHandler* handler);

    /**
     * Attach the specified camera to this viewport.
     */
    void SetCamera(CameraNode* camera_node);

    CameraNode* GetCamera() { return camera_; }

    void ActivateInputHandler(InputHandler* handler);

    Scene::Ptr GetScene() { return scene_; }

    ResourceManager::Ptr GetResources() { return resources_; }

    std::vector<Renderer*> GetRenderers() { return renderers_; }

    std::vector<InputHandler*> GetInputHandlers() { return input_handlers_; }

 public slots:
    void ScheduleRedraw();

 signals:
    void RendererAdded(Renderer* renderer);

    void InputHandlerAdded(InputHandler* handler);

    void InputHandlerActivated(InputHandler* handler);

    void CameraChanged(CameraNode* camera_node);

 protected:
    void initializeGL() override;

    void resizeGL(int width, int height) override;

    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

 private slots:
    void Render();

 private:
    void PrepareFixedFunctionPipeline();

    ResourceManager::Ptr resources_;

    Scene::Ptr scene_;

    CameraNode* camera_;

    InputHandler* input_handler_;

    std::unique_ptr<DrawScene> draw_;

    std::vector<Renderer*> renderers_;

    std::vector<InputHandler*> input_handlers_;

    bool redraw_scheduled_;

    QOpenGLContext* m_context_;

    QColor background_color_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_VIEWPORT_HPP__
