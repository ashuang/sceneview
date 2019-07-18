// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_VIEWPORT_HPP__
#define SCENEVIEW_VIEWPORT_HPP__

#include <vector>

#include <QOpenGLWidget>

#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

namespace sv {

class DrawContext;
class DrawGroup;
class CameraNode;
class InputHandler;
class Renderer;

/**
 * Widget that draws a scene and manages Renderer and InputHandler objects.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/viewport.hpp
 */
class Viewport : public QOpenGLWidget {
  Q_OBJECT

 public:
  explicit Viewport(const ResourceManager::Ptr& resources,
                    const Scene::Ptr& scene, QWidget* parent = nullptr);

  Viewport(const Viewport&) = delete;

  Viewport& operator=(const Viewport&) = delete;

  ~Viewport();

  void AddRenderer(Renderer* renderer);

  void AddInputHandler(InputHandler* handler);

  /**
   * Attach the specified camera to this viewport.
   *
   * More specifically, sets the camera for the attached scene's default draw
   * group.
   */
  void SetCamera(CameraNode* camera_node);

  CameraNode* GetCamera();

  void ActivateInputHandler(InputHandler* handler);

  Scene::Ptr GetScene();

  ResourceManager::Ptr GetResources();

  std::vector<Renderer*> GetRenderers();

  std::vector<InputHandler*> GetInputHandlers();

  void SetBackgroundColor(const QColor& color);

  void SetDrawGroups(const std::vector<DrawGroup*>& groups);

  InputHandler* GetActiveInputHandler();

 public slots:
  void ScheduleRedraw();

 signals:
  void RendererAdded(Renderer* renderer);

  void InputHandlerAdded(InputHandler* handler);

  void InputHandlerActivated(InputHandler* handler);

  void CameraChanged(CameraNode* camera_node);

  void GLInitialized();

  void GLShuttingDown();

 protected:
  void initializeGL() override;

  void resizeGL(int width, int height) override;

  void paintGL() override;

  void mousePressEvent(QMouseEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void mouseReleaseEvent(QMouseEvent* event) override;

  void mouseDoubleClickEvent(QMouseEvent* event) override;

  void wheelEvent(QWheelEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override;

  void keyReleaseEvent(QKeyEvent* event) override;

 private slots:
  void Render();

 private:
  struct Priv;
  Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_VIEWPORT_HPP__
