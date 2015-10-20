// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_VIEWER_HPP__
#define SCENEVIEW_VIEWER_HPP__

#include <map>
#include <memory>

#include <QMainWindow>
#include <QTimer>

#include <sceneview/viewport.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

class QAction;
class QActionGroup;
class QCloseEvent;
class QDockWidget;
class QMenu;
class QMenuBar;
class QSettings;

namespace sceneview {

class RendererWidgetStack;
class InputHandlerWidget;
class ViewHandler;

class Viewer : public QMainWindow {
  Q_OBJECT

  public:
    explicit Viewer(QWidget* parent = nullptr);

    Viewport* GetViewport() { return viewport_; }

    void SetAutoRedrawInterval(int milliseconds);

    QMenu* FileMenu() { return file_menu_; }

    void SaveSettings(QSettings* settings);

    void LoadSettings(QSettings* settings);

  signals:
    /**
     * Emitted when the window is about to close.
     */
    void Closing();

  private slots:
    void OnRendererAdded(Renderer* renderer);

    void OnInputHandlerAdded(InputHandler* handler);

    void OnInputHandlerActivated(InputHandler* handler);

  private:
    void CreateMenus();

    void closeEvent(QCloseEvent* event) override;

    void SetupShowHideAction(QAction* action, QDockWidget* widget);

    ResourceManager::Ptr resources_;
    Scene::Ptr default_scene_;
    Viewport* viewport_;

    RendererWidgetStack* renderer_widget_stack_;
    InputHandlerWidget* input_handler_widget_;

    QTimer redraw_timer_;

    QMenu* file_menu_;
    QMenu* renderer_menu_;
    QMenu* view_menu_;
    QMenu* input_handler_menu_;
    QActionGroup* input_action_group_;
    std::map<InputHandler*, QAction*> input_handler_actions_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_VIEWER_HPP__
