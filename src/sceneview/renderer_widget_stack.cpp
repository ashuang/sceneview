// Copyright [2015] Albert Huang

#include "sceneview/renderer_widget_stack.hpp"

#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "sceneview/expander_widget.hpp"
#include "sceneview/renderer.hpp"

namespace sv {

struct RendererWidgetStack::Priv {
  QScrollArea* area;
  QWidget* container;
  QVBoxLayout* layout;
};

RendererWidgetStack::RendererWidgetStack(QWidget* parent)
    : QDockWidget(parent), p_(new Priv) {
  setObjectName("sceneview/renderer_widget_stack");

  p_->container = new QWidget(this);
  p_->layout = new QVBoxLayout(p_->container);

  p_->area = new QScrollArea(this);
  p_->area->setWidget(p_->container);
  p_->container->setMinimumSize(200, 50);
  setWidget(p_->area);
  p_->layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  p_->layout->setSpacing(0);
  p_->layout->setContentsMargins(2, 2, 2, 2);

  p_->layout->addStretch();

  setWindowTitle("Renderers");
}

RendererWidgetStack::~RendererWidgetStack() { delete p_; }

void RendererWidgetStack::AddRendererWidget(Renderer* renderer) {
  QWidget* rwidget = renderer->GetWidget();
  if (!rwidget) {
    return;
  }

  // first, remove the stretch item at the end. We'll put it back in after
  // inserting the new widget.
  p_->layout->removeItem(p_->layout->itemAt(p_->layout->count() - 1));

  // Create an expander widget to house the new renderer widget.
  ExpanderWidget* expander = new ExpanderWidget(this);
  expander->SetWidget(rwidget);
  expander->SetTitle(renderer->Name());
  expander->SetExpanded(false);
  renderer->setProperty("sceneview/expander_widget",
                        QVariant::fromValue(expander));
  p_->layout->addWidget(expander, 0, Qt::AlignTop);

  // Add a stretch item at the end to fill up empty space.
  p_->layout->addStretch();
}

ExpanderWidget* RendererWidgetStack::GetExpanderWidget(Renderer* renderer) {
  QVariant expander_variant = renderer->property("sceneview/expander_widget");
  if (!expander_variant.isValid()) {
    return nullptr;
  }
  return expander_variant.value<ExpanderWidget*>();
}

}  // namespace sv
