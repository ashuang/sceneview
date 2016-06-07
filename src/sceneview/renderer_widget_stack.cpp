// Copyright [2015] Albert Huang

#include "sceneview/renderer_widget_stack.hpp"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>

#include "sceneview/expander_widget.hpp"
#include "sceneview/renderer.hpp"

namespace sv {

RendererWidgetStack::RendererWidgetStack(QWidget* parent) :
  QDockWidget(parent) {
  setObjectName("sceneview/renderer_widget_stack");

  container_ = new QWidget(this);
  layout_ = new QVBoxLayout(container_);

  area_ = new QScrollArea(this);
  area_->setWidget(container_);
  container_->setMinimumSize(200, 50);
  setWidget(area_);
  layout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
  layout_->setSpacing(0);
  layout_->setContentsMargins(2, 2, 2, 2);

  layout_->addStretch();

  setWindowTitle("Renderers");
}

void RendererWidgetStack::AddRendererWidget(Renderer* renderer) {
  QWidget* rwidget = renderer->GetWidget();
  if (!rwidget) {
    return;
  }

  // first, remove the stretch item at the end. We'll put it back in after
  // inserting the new widget.
  layout_->removeItem(layout_->itemAt(layout_->count() - 1));

  // Create an expander widget to house the new renderer widget.
  ExpanderWidget* expander = new ExpanderWidget(this);
  expander->SetWidget(rwidget);
  expander->SetTitle(renderer->Name());
  expander->SetExpanded(false);
  renderer->setProperty("sceneview/expander_widget", QVariant::fromValue(expander));
  layout_->addWidget(expander, 0, Qt::AlignTop);

  // Add a stretch item at the end to fill up empty space.
  layout_->addStretch();
}

ExpanderWidget* RendererWidgetStack::GetExpanderWidget(Renderer* renderer) {
  QVariant expander_variant = renderer->property("sceneview/expander_widget");
  if (!expander_variant.isValid()) {
    return nullptr;
  }
  return expander_variant.value<ExpanderWidget*>();
}

}  // namespace sv
