// Copyright [2015] Albert Huang

#include "sceneview/param_widget.hpp"

#include <cassert>
#include <cmath>
#include <vector>

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QMap>
#include <QString>

namespace sceneview {

ParamWidget::ParamWidget(QWidget* parent) :
 layout_(new QVBoxLayout(this)),
 widgets_() {
  layout_->setSpacing(0);
  layout_->setContentsMargins(2, 2, 2, 2);
}

void ParamWidget::AddEnum(const QString& name,
    const EnumVector& items,
    int initial_value,
    DisplayHint display_hint) {
  ExpectNameNotFound(name);

  if (display_hint != DisplayHint::kComboBox) {
    throw std::invalid_argument("Invalid display hint");
  }

  QComboBox* combobox = new QComboBox(this);
  combobox->setProperty("param_widget_type", kParamEnum);
  bool found_initial_value = false;
  for (size_t i = 0; i < items.size(); ++i) {
    const EnumItem& item = items[i];
    combobox->addItem(item.first, item.second);
    if (item.second == initial_value) {
      found_initial_value = true;
      combobox->setCurrentIndex(i);
    }
  }
  if (!found_initial_value) {
    throw std::invalid_argument("Invalid initial value");
  }
  widgets_[name] = combobox;
  AddLabeledRow(name, combobox);
  connect(combobox,
      static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
      [this, name](int val) {
        emit ParamChanged(name);
      });
}

void ParamWidget::AddBoolean(const QString& name, bool initial_value,
    DisplayHint display_hint) {
  AddBooleans({{name, initial_value}}, display_hint);
}

void ParamWidget::AddBooleans(const std::vector<BoolItem>& to_add,
    DisplayHint display_hint) {
  if (display_hint != kCheckBox) {
    throw std::invalid_argument("Invalid display hint");
  }

  QWidget* row_widget = new QWidget(this);
  QHBoxLayout* hbox = new QHBoxLayout(row_widget);
  for (const BoolItem& item : to_add) {
    ExpectNameNotFound(item.name);

    QCheckBox* checkbox =
      new QCheckBox(item.name, this);

    if (item.initially_checked) {
      checkbox->setCheckState(Qt::Checked);
    } else {
     checkbox->setCheckState(Qt::Unchecked);
    }
    checkbox->setProperty("param_widget_type", kParamBool);

    widgets_[item.name] = checkbox;
    hbox->addWidget(checkbox);

    connect(checkbox, &QCheckBox::stateChanged,
        [this, item](int val) { emit ParamChanged(item.name); });
  }

  layout_->addWidget(row_widget);
}

void ParamWidget::AddInt(const QString& name,
    int min, int max, int step, int initial_value,
    DisplayHint display_hint) {
  ExpectNameNotFound(name);

  if (display_hint == DisplayHint::kSpinBox) {
    QSpinBox* spinbox = new QSpinBox(this);
    spinbox->setRange(min, max);
    spinbox->setSingleStep(step);
    spinbox->setValue(initial_value);
    spinbox->setProperty("param_widget_type", kParamInt);
    widgets_[name] = spinbox;
    AddLabeledRow(name, spinbox);
    connect(spinbox,
        static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        [this, name](int val) {
          emit ParamChanged(name);
        });
  } else if (display_hint == DisplayHint::kSlider) {
    QWidget* row_widget = new QWidget(this);
    QHBoxLayout* row_hbox = new QHBoxLayout(row_widget);
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);
    slider->setSingleStep(step);
    slider->setValue(initial_value);
    slider->setProperty("param_widget_type", kParamInt);
    QLabel* label = new QLabel(this);
    label->setText(QString::number(initial_value));
    row_hbox->addWidget(new QLabel(name, this));
    row_hbox->addWidget(slider);
    row_hbox->addWidget(label);
    widgets_[name] = slider;
    layout_->addWidget(row_widget);
    connect(slider, &QSlider::valueChanged,
        [this, name, label](int value) {
          label->setText(QString::number(value));
          emit ParamChanged(name);
        });
  } else {
    throw std::invalid_argument("Invalid display hint");
  }
}

void ParamWidget::AddDouble(const QString& name,
    double min, double max, double step, double initial_value,
    DisplayHint display_hint) {
  ExpectNameNotFound(name);

  if (display_hint == DisplayHint::kSpinBox) {
    QDoubleSpinBox* spinbox = new QDoubleSpinBox(this);
    spinbox->setRange(min, max);
    spinbox->setSingleStep(step);
    spinbox->setValue(initial_value);
    spinbox->setProperty("param_widget_type", kParamDouble);
    widgets_[name] = spinbox;
    AddLabeledRow(name, spinbox);
    connect(spinbox,
        static_cast<void(QDoubleSpinBox::*)(double)>(
          &QDoubleSpinBox::valueChanged),
        [this, name](double value) {
          emit ParamChanged(name);
        });
  } else if (display_hint == DisplayHint::kSlider) {
    QWidget* row_widget = new QWidget(this);
    QHBoxLayout* row_hbox = new QHBoxLayout(row_widget);
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    const int num_steps = static_cast<int>((max - min) / step);
    const int initial_value_int =
      static_cast<int>((initial_value - min) / step);
    slider->setRange(0, num_steps);
    slider->setSingleStep(1);
    slider->setValue(initial_value_int);
    slider->setProperty("min", min);
    slider->setProperty("max", max);
    slider->setProperty("step", step);
    slider->setProperty("param_widget_type", kParamDouble);
    QLabel* label = new QLabel(this);
    label->setText(QString::number((initial_value_int - min) * step));
    row_hbox->addWidget(new QLabel(name, this));
    row_hbox->addWidget(slider);
    row_hbox->addWidget(label);
    widgets_[name] = slider;
    layout_->addWidget(row_widget);
    connect(slider, &QSlider::valueChanged,
        [this, name, label, min, step](int position) {
          const double value = min + step * position;
          label->setText(QString::number(value));
          emit ParamChanged(name);
        });
  } else {
    throw std::invalid_argument("Invalid display hint");
  }
}

void ParamWidget::AddPushButton(const QString& name) {
  AddPushButtons({name});
}

void ParamWidget::AddPushButtons(const std::vector<QString>& names) {
  QWidget* row_widget = new QWidget(this);
  QHBoxLayout* hbox = new QHBoxLayout(row_widget);
  for (const QString& name : names) {
    QPushButton* button =
      new QPushButton(name, this);
    button->setProperty("param_widget_type", kParamButton);
    hbox->addWidget(button);
    connect(button, &QPushButton::clicked,
        [this, name](bool ignored) { emit ParamChanged(name); });
    widgets_[name] = button;
  }
  layout_->addWidget(row_widget);
}

int ParamWidget::GetEnum(const QString& name) {
  QComboBox* combobox = dynamic_cast<QComboBox*>(GetWidget(name));
  if (!combobox) {
    throw std::invalid_argument("Invalid enum parameter " + name.toStdString());
  }
  return combobox->currentData().toInt();
}

QString ParamWidget::GetEnumString(const QString& name) {
  QComboBox* combobox = dynamic_cast<QComboBox*>(GetWidget(name));
  if (!combobox) {
    throw std::invalid_argument("Invalid enum parameter " + name.toStdString());
  }
  return combobox->currentText();
}

bool ParamWidget::GetBool(const QString& name) {
  QCheckBox* checkbox = dynamic_cast<QCheckBox*>(GetWidget(name));
  if (!checkbox) {
    throw std::invalid_argument("Invalid bool parameter " + name.toStdString());
  }
  return checkbox->checkState() == Qt::Checked;
}

int ParamWidget::GetInt(const QString& name) {
  QWidget* widget = GetWidget(name);
  QSpinBox* spinbox = dynamic_cast<QSpinBox*>(widget);
  if (spinbox) {
    return spinbox->value();
  }
  QSlider* slider = dynamic_cast<QSlider*>(widget);
  if (slider) {
    return slider->value();
  }
  throw std::runtime_error("Unable to determine widget type for param " +
      name.toStdString());
}

double ParamWidget::GetDouble(const QString& name) {
  QWidget* widget = GetWidget(name);
  QDoubleSpinBox* spinbox = dynamic_cast<QDoubleSpinBox*>(widget);
  if (spinbox) {
    return spinbox->value();
  }
  QSlider* slider = dynamic_cast<QSlider*>(widget);
  if (slider) {
    const double min = slider->property("min").toDouble();
    const double step = slider->property("step").toDouble();
    return min + step * slider->value();
  }
  throw std::runtime_error("Unable to determine widget type for param " +
      name.toStdString());
}

void ParamWidget::SetEnum(const QString& name, int val) {
  QComboBox* combobox = dynamic_cast<QComboBox*>(GetWidget(name));
  if (!combobox) {
    throw std::invalid_argument("Invalid enum parameter " + name.toStdString());
  }
  int index = combobox->findData(val);
  if (index >= 0) {
    combobox->setCurrentIndex(index);
  } else {
    throw std::invalid_argument("Invalid value for enum " + name.toStdString());
  }
}

void ParamWidget::SetBool(const QString& name, bool val) {
  QCheckBox* checkbox = dynamic_cast<QCheckBox*>(GetWidget(name));
  if (!checkbox) {
    throw std::invalid_argument("Invalid bool parameter " + name.toStdString());
  }
  if (val) {
    checkbox->setCheckState(Qt::Checked);
  } else {
    checkbox->setCheckState(Qt::Unchecked);
  }
}

void ParamWidget::SetInt(const QString& name, int val) {
  QWidget* widget = GetWidget(name);
  QSpinBox* spinbox = dynamic_cast<QSpinBox*>(widget);
  if (spinbox) {
    spinbox->setValue(val);
    return;
  }
  QSlider* slider = dynamic_cast<QSlider*>(widget);
  if (slider) {
    slider->setValue(val);
    return;
  }
  throw std::runtime_error("Unable to determine widget type for param " +
      name.toStdString());
}

void ParamWidget::SetDouble(const QString& name, double val) {
  QWidget* widget = GetWidget(name);
  QDoubleSpinBox* spinbox = dynamic_cast<QDoubleSpinBox*>(widget);
  if (spinbox) {
    spinbox->setValue(val);
    return;
  }
  QSlider* slider = dynamic_cast<QSlider*>(widget);
  if (slider) {
    const double min = slider->property("min").toDouble();
    const double step = slider->property("step").toDouble();
    const int position = static_cast<int>(round((val - min) / step));
    slider->setValue(position);
    return;
  }
  throw std::runtime_error("Unable to determine widget type for param " +
      name.toStdString());
}

QVariant ParamWidget::SaveState() {
  QMap<QString, QVariant> data;

  for (auto& item : widgets_) {
    const QString& name = item.first;
    const QString qname = name;
    QWidget* widget = item.second;
    QVariant param_type = widget->property("param_widget_type");
    assert(param_type.isValid());
    switch (param_type.toInt()) {
      case kParamEnum:
        data[qname] = GetEnum(name);
        break;
      case kParamBool:
        data[qname] = GetBool(name);
        break;
      case kParamInt:
        data[qname] = GetInt(name);
        break;
      case kParamDouble:
        data[qname] = GetDouble(name);
        break;
      case kParamButton:
      default:
        assert(false);
        break;
    }
  }
  return data;
}

void ParamWidget::LoadState(const QVariant& variant) {
  QMap<QString, QVariant> data = variant.toMap();

  for (auto& item : widgets_) {
    const QString& name = item.first;
    const QString qname = name;
    QWidget* widget = item.second;
    QVariant param_type = widget->property("param_widget_type");
    QVariant value = data[qname];
    assert(param_type.isValid());
    switch (param_type.toInt()) {
      case kParamEnum:
        if (value.canConvert<int>()) {
          SetEnum(name, value.toInt());
        }
        break;
      case kParamBool:
        if (value.canConvert<bool>()) {
          SetBool(name, value.toBool());
        }
        break;
      case kParamInt:
        if (value.canConvert<int>()) {
          SetInt(name, value.toInt());
        }
        break;
      case kParamDouble:
        if (value.canConvert<double>()) {
          SetDouble(name, value.toDouble());
        }
        break;
      case kParamButton:
      default:
        assert(false);
        break;
    }
  }
}

void ParamWidget::SetEnabled(const QString& name, bool enabled) {
  QWidget* widget = GetWidget(name);
  widget->setEnabled(enabled);
}

void ParamWidget::ExpectNameNotFound(const QString& name) {
  if (widgets_.find(name) != widgets_.end()) {
    throw std::invalid_argument("Duplicate parameter name " +
        name.toStdString());
  }
}

void ParamWidget::AddLabeledRow(const QString& name, QWidget* widget) {
  QLabel* label = new QLabel(name, this);
  QWidget* row_widget = new QWidget(this);
  QHBoxLayout* hboxlayout = new QHBoxLayout(row_widget);
  hboxlayout->addWidget(label);
  hboxlayout->addWidget(widget);
  layout_->addWidget(row_widget);
}

QWidget* ParamWidget::GetWidget(const QString& name) {
  auto iter = widgets_.find(name);
  if (iter == widgets_.end()) {
    throw std::invalid_argument("No such parameter name " + name.toStdString());
  }
  return iter->second;
}

}  // namespace sceneview
