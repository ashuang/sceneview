// Copyright [2015] Albert Huang

#include "sceneview/expander_widget.hpp"

#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace sv {

  struct ExpanderWidget::Priv {
    QPushButton* button;
    QWidget* widget;
    QVBoxLayout* layout;
    bool expanded;
  };

ExpanderWidget::ExpanderWidget(QWidget* parent) :
  QWidget(parent),
  p_(new Priv) {

  p_->button = new QPushButton(this);
  p_->widget = nullptr;
  p_->layout = new QVBoxLayout(this);
  p_->expanded = true;
  p_->button->setText("");
  p_->button->setFlat(true);
  p_->button->setStyleSheet("text-align: left; border: none;");
  p_->button->setIcon(QIcon(":/sceneview/icons/arrow_down.png"));
  connect(p_->button, &QPushButton::clicked, this,
      &ExpanderWidget::ToggleExpanded);

  p_->layout->addWidget(p_->button, 0, Qt::AlignTop);
  p_->layout->setSpacing(0);
  p_->layout->setContentsMargins(1, 1, 1, 1);
}

ExpanderWidget::~ExpanderWidget() {
  delete p_;
}

void ExpanderWidget::SetWidget(QWidget* widget) {
  if (p_->widget) {
    p_->layout->removeWidget(p_->widget);
  }
  p_->layout->addWidget(widget);
  p_->widget = widget;

  if (p_->expanded) {
    p_->widget->show();
  } else {
    p_->widget->hide();
  }
}

QSize ExpanderWidget::sizeHint() const {
  return QSize(200, 20);
//  if (p_->widget && p_->expanded) {
//    return p_->layout->sizeHin
//  } else {
//    return p_->button->sizeHint();
//  }
}

void ExpanderWidget::ToggleExpanded() {
  p_->expanded = !p_->expanded;

  if (p_->expanded) {
    if (p_->widget) {
      p_->layout->addWidget(p_->widget);
      p_->widget->show();
    }
    p_->button->setIcon(QIcon(":/sceneview/icons/arrow_down.png"));
    updateGeometry();
  } else {
    if (p_->widget) {
      p_->widget->hide();
      p_->layout->removeWidget(p_->widget);
    }
    p_->button->setIcon(QIcon(":/sceneview/icons/arrow_right.png"));
    updateGeometry();
  }
}

void ExpanderWidget::SetTitle(const QString& title) {
  p_->button->setText(title);
}

QString ExpanderWidget::Title() const {
  return p_->button->text();
}

void ExpanderWidget::SetExpanded(bool val) {
  if (val != p_->expanded) {
    ToggleExpanded();
  }
}

bool ExpanderWidget::Expanded() const { return p_->expanded; }

}  // namespace sv
