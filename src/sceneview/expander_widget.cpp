// Copyright [2015] Albert Huang

#include "sceneview/expander_widget.hpp"

#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace sv {

ExpanderWidget::ExpanderWidget(QWidget* parent) :
  QWidget(parent),
  button_(new QPushButton(this)),
  widget_(nullptr),
  layout_(new QVBoxLayout(this)),
  expanded_(true) {
  button_->setText("");
  button_->setFlat(true);
  button_->setStyleSheet("text-align: left; border: none;");
  button_->setIcon(QIcon(":/sceneview/icons/arrow_down.png"));
  connect(button_, &QPushButton::clicked, this,
      &ExpanderWidget::ToggleExpanded);

  layout_->addWidget(button_, 0, Qt::AlignTop);
  layout_->setSpacing(0);
  layout_->setContentsMargins(1, 1, 1, 1);
}

void ExpanderWidget::SetWidget(QWidget* widget) {
  if (widget_) {
    layout_->removeWidget(widget_);
  }
  layout_->addWidget(widget);
  widget_ = widget;

  if (expanded_) {
    widget_->show();
  } else {
    widget_->hide();
  }
}

QSize ExpanderWidget::sizeHint() const {
  return QSize(200, 20);
//  if (widget_ && expanded_) {
//    return layout_->sizeHin
//  } else {
//    return button_->sizeHint();
//  }
}

void ExpanderWidget::ToggleExpanded() {
  expanded_ = !expanded_;

  QSize sz = size();
  if (expanded_) {
    if (widget_) {
      layout_->addWidget(widget_);
      widget_->show();
    }
    button_->setIcon(QIcon(":/sceneview/icons/arrow_down.png"));
    updateGeometry();
  } else {
    if (widget_) {
      widget_->hide();
      layout_->removeWidget(widget_);
    }
    button_->setIcon(QIcon(":/sceneview/icons/arrow_right.png"));
    updateGeometry();
  }
}

void ExpanderWidget::SetTitle(const QString& title) {
  button_->setText(title);
}

QString ExpanderWidget::Title() const {
  return button_->text();
}

void ExpanderWidget::SetExpanded(bool val) {
  if (val != expanded_) {
    ToggleExpanded();
  }
}

}  // namespace sv
