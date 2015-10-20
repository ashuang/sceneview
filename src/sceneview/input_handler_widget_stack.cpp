// Copyright [2015] Albert Huang

#include "sceneview/input_handler_widget_stack.hpp"

#include <QStackedWidget>
#include <QVBoxLayout>

#include "sceneview/input_handler.hpp"
#include "sceneview/view_handler_horizontal.hpp"
#include "sceneview/viewport.hpp"

namespace sv {

InputHandlerWidgetStack::InputHandlerWidgetStack(Viewport* viewport,
    QWidget* parent) :
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
      this, &InputHandlerWidgetStack::OnInputHandlerActivated);

  connect(viewport_, &Viewport::InputHandlerAdded,
      this, &InputHandlerWidgetStack::AddInputHandler);
}

void InputHandlerWidgetStack::AddInputHandler(InputHandler* handler) {
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

void InputHandlerWidgetStack::OnInputHandlerActivated(InputHandler* handler) {
  setWindowTitle("Input (" + handler->Name() + ")");

  HandlerData* hdata = GetHandlerData(handler);
  stack_->setCurrentIndex(hdata->stack_index);
}

InputHandlerWidgetStack::HandlerData* InputHandlerWidgetStack::GetHandlerData(
    InputHandler* handler) {
  for (HandlerData& hdata : handler_data_) {
    if (hdata.handler == handler) {
      return &hdata;
    }
  }
  return nullptr;
}

}  // namespace sv
