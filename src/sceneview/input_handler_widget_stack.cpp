// Copyright [2015] Albert Huang

#include "sceneview/input_handler_widget_stack.hpp"

#include <QStackedWidget>
#include <QVBoxLayout>

#include "sceneview/input_handler.hpp"
#include "sceneview/view_handler_horizontal.hpp"
#include "sceneview/viewport.hpp"

namespace sv {

struct InputHandlerWidgetStack::Priv {
  Viewport* viewport;

  QStackedWidget* stack;
  QVBoxLayout* layout;

  std::vector<HandlerData> handler_data;
};

InputHandlerWidgetStack::InputHandlerWidgetStack(Viewport* viewport,
                                                 QWidget* parent)
    : QDockWidget(parent), p_(new Priv()) {
  p_->viewport = viewport;
  setObjectName("sceneview/input_handler_stack");

  setWindowTitle("Input");

  // Setup the widget
  QWidget* container = new QWidget(this);
  setWidget(container);
  p_->layout = new QVBoxLayout(container);
  p_->stack = new QStackedWidget(this);
  p_->layout->addWidget(p_->stack);

  for (InputHandler* handler : p_->viewport->GetInputHandlers()) {
    AddInputHandler(handler);
  }

  connect(p_->viewport, &Viewport::InputHandlerActivated, this,
          &InputHandlerWidgetStack::OnInputHandlerActivated);

  connect(p_->viewport, &Viewport::InputHandlerAdded, this,
          &InputHandlerWidgetStack::AddInputHandler);
}

InputHandlerWidgetStack::~InputHandlerWidgetStack() { delete p_; }

void InputHandlerWidgetStack::AddInputHandler(InputHandler* handler) {
  for (const HandlerData& hdata : p_->handler_data) {
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
  hdata.stack_index = p_->stack->addWidget(widget);
  hdata.handler = handler;

  p_->handler_data.push_back(hdata);
}

void InputHandlerWidgetStack::OnInputHandlerActivated(InputHandler* handler) {
  setWindowTitle("Input (" + handler->Name() + ")");

  HandlerData* hdata = GetHandlerData(handler);
  p_->stack->setCurrentIndex(hdata->stack_index);
}

InputHandlerWidgetStack::HandlerData* InputHandlerWidgetStack::GetHandlerData(
    InputHandler* handler) {
  for (HandlerData& hdata : p_->handler_data) {
    if (hdata.handler == handler) {
      return &hdata;
    }
  }
  return nullptr;
}

}  // namespace sv
