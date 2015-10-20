#ifndef SCENEVIEW_INPUT_HANDLER_WIDGET
#define SCENEVIEW_INPUT_HANDLER_WIDGET

#include <QDockWidget>

#include <sceneview/input_handler.hpp>

class QComboBox;
class QStackedWidget;
class QVBoxLayout;

namespace sceneview {

class Viewport;

class InputHandlerWidget : public QDockWidget {
  Q_OBJECT

  public:
    InputHandlerWidget(Viewport* viewport, QWidget* parent = nullptr);

  private:
    struct HandlerData {
      int stack_index;
      InputHandler* handler;
    };

    void AddInputHandler(InputHandler* handler);

    void OnInputHandlerActivated(InputHandler* handler);

    HandlerData* GetHandlerData(InputHandler* Handler);

    Viewport* viewport_;

    QStackedWidget* stack_;
    QVBoxLayout* layout_;

    std::vector<HandlerData> handler_data_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_CAMERA_CONTROLS_WIDGET
