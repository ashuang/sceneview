// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_INPUT_HANDLER_WIDGET
#define SCENEVIEW_INPUT_HANDLER_WIDGET

#include <vector>

#include <QDockWidget>

namespace sv {

class InputHandler;
class Viewport;

/**
 * Contains and manages InputHandler widgets.
 *
 * When an InputHandler provides a widget by overriding
 * InputHandler::GetWidget(), the Viewer class places them inside an
 * InputHandlerWidgetStack widget.
 *
 * Sceneview users generally will not have to interact with this class.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/input_handler_widget_stack.hpp
 */
class InputHandlerWidgetStack : public QDockWidget {
  Q_OBJECT

 public:
  explicit InputHandlerWidgetStack(Viewport* viewport,
                                   QWidget* parent = nullptr);

  ~InputHandlerWidgetStack();

 private:
  struct HandlerData {
    int stack_index;
    InputHandler* handler;
  };

  void AddInputHandler(InputHandler* handler);

  void OnInputHandlerActivated(InputHandler* handler);

  HandlerData* GetHandlerData(InputHandler* Handler);

  struct Priv;

  Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_CAMERA_CONTROLS_WIDGET
