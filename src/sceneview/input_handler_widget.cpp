#include <sceneview/input_handler_widget.hpp>

#include <QStackedWidget>
#include <QVBoxLayout>

#include <sceneview/input_handler.hpp>
#include <sceneview/view_handler_horizontal.hpp>
#include <sceneview/viewport.hpp>

namespace sceneview {

InputHandlerWidget::InputHandlerWidget(Viewport* viewport, QWidget* parent) :
  QDockWidget(parent),
  viewport_(viewport) {
  setObjectName("sceneview/input_handler_stack");

  setWindowTitle("Input");

  // Setup the widget
  QWidget* container = new QWidget(this);
  setWidget(container);
  layout_ = new QVBoxLayout(container);
  stack_ = new QStackedWidget(this);
  layout_->addWidget(stack_);

  for (InputHandler* handler : viewport_->GetInputHandlers()) {
    AddInputHandler(handler);
  }

  connect(viewport_, &Viewport::InputHandlerActivated,
      this, &InputHandlerWidget::OnInputHandlerActivated);

  connect(viewport_, &Viewport::InputHandlerAdded,
      this, &InputHandlerWidget::AddInputHandler);
}

void InputHandlerWidget::AddInputHandler(InputHandler* handler) {
  for (const HandlerData& hdata : handler_data_) {
    if (handler->Name() == hdata.handler->Name()) {
      throw std::invalid_argument("Duplicate input handlers named " +
          handler->Name().toStdString());
    }
  }

  QWidget* widget = handler->GetWidget();
  if (!widget) {
    widget = new QWidget(this);
  }

  HandlerData hdata;
  hdata.stack_index = stack_->addWidget(widget);
  hdata.handler = handler;

  handler_data_.push_back(hdata);
}

void InputHandlerWidget::OnInputHandlerActivated(InputHandler* handler) {
  setWindowTitle("Input (" + handler->Name() + ")");

  HandlerData* hdata = GetHandlerData(handler);
  stack_->setCurrentIndex(hdata->stack_index);
}

InputHandlerWidget::HandlerData* InputHandlerWidget::GetHandlerData(
    InputHandler* handler) {
  for (HandlerData& hdata : handler_data_) {
    if (hdata.handler == handler) {
      return &hdata;
    }
  }
  return nullptr;
}

}  // namespace sceneview
