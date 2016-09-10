// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_PARAM_WIDGET_HPP__
#define SCENEVIEW_PARAM_WIDGET_HPP__

#include <map>
#include <utility>
#include <vector>

#include <QWidget>
#include <QVBoxLayout>
#include <QVariant>

class QCheckBox;
class QSpinBox;
class QSlider;

namespace sv {

/**
 * A convenience widget to enable quickly exposing tunable parameters.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/param_widget.hpp
 */
class ParamWidget : public QWidget {
  Q_OBJECT

  public:
    enum DisplayHint {
      kComboBox,
      kCheckBox,
      kSlider,
      kSpinBox
    };

    typedef std::pair<QString, int> EnumItem;
    typedef std::vector<EnumItem> EnumVector;

    explicit ParamWidget(QWidget* parent = nullptr);

    struct BoolItem {
      QString name;
      bool initially_checked;
    };

    /**
     * Adds an enum parameter.
     *
     * @param name parameter name.
     * @param items the list of options.
     * @param initial_value the initially selected option.
     * @param display_hint currently, must be ParamWidget::kComboBox.
     *
     * When the value of the parameter changes, the ParamChanged() signal is
     * emitted.
     */
    void AddEnum(const QString& name,
        const EnumVector& items,
        int initial_value,
        DisplayHint display_hint);

    /**
     * Adds a boolean parameter.
     *
     * @param name parameter name.
     * @param initial_value the initial value of the boolean
     * @param display_hint currently, must be ParamWidget::kCheckBox
     *
     * When the value of the parameter changes, the ParamChanged() signal is
     * emitted.
     */
    void AddBoolean(const QString& name, bool initial_value,
        DisplayHint display_hint);

    /**
     * Adds one or more boolean parameters all in the same row.
     *
     * @param to_add A list of boolean parameters to add.
     * @param display_hint currently, must be ParamWidget::kCheckBox
     *
     * The difference between calling AddBooleans() and calling AddBoolean()
     * multiple times is that all of the parameters passed this method will be
     * displayed on the same row if possible in the parameter widget. When
     * calling AddBoolean() multiple times, each parameter is allocated a new
     * row.
     *
     * When the value of any of the parameters changes, the ParamChanged()
     * signal is emitted.
     */
    void AddBooleans(const std::vector<BoolItem>& to_add,
        DisplayHint display_hint);

    /**
     * Adds an int parameter.
     *
     * @param name parameter name.
     * @param min the minimum value
     * @param max the maximum value
     * @param step the increment between valid values
     * @param display_hint must be either ParamWidget::kSlider or ParamWidget::kSpinBox
     *
     * When the value of the parameter changes, the ParamChanged() signal is
     * emitted.
     */
    void AddInt(const QString& name,
        int min, int max, int step,
        int initial_value,
        DisplayHint display_hint);

    /**
     * Adds a double parameter.
     *
     * @param name parameter name.
     * @param min the minimum value
     * @param max the maximum value
     * @param step the increment between valid values
     * @param display_hint must be either ParamWidget::kSlider or ParamWidget::kSpinBox
     *
     * When the value of the parameter changes, the ParamChanged() signal is
     * emitted.
     */
    void AddDouble(const QString& name,
        double min, double max, double step,
        double initial_value,
        DisplayHint display_hint);

    /**
     * Adds a push button.
     *
     * @param name the button name.
     *
     * When the button is clicked, the ParamChanged() signal is emitted.
     */
    void AddPushButton(const QString& name);

    /**
     * Adds one or more push buttons all in the same row.
     *
     * @param names the button names.
     *
     * When any of the buttons are clicked, the ParamChanged() signal is emitted.
     */
    void AddPushButtons(const std::vector<QString>& names);

    int GetEnum(const QString& name);

    QString GetEnumString(const QString& name);

    bool GetBool(const QString& name);

    int GetInt(const QString& name);

    double GetDouble(const QString& name);

    void SetEnum(const QString& name, int val);

    void SetBool(const QString& name, bool val);

    void SetInt(const QString& name, int val);

    void SetDouble(const QString& name, double val);

    void SetPrecision(const QString& name, int digits, int decimal_places);

    void SetEnabled(const QString& name, bool enabled);

    QVariant SaveState();

    void LoadState(const QVariant& variant);

  signals:
    void ParamChanged(const QString& name);

  private:
    enum ParamType {
      kParamEnum,
      kParamBool,
      kParamInt,
      kParamDouble,
      kParamButton
    };

    void ExpectNameNotFound(const QString& name);

    void AddLabeledRow(const QString& name, QWidget* widget);

    QWidget* GetWidget(const QString& name);

    QVBoxLayout* layout_;

    std::map<QString, QWidget*> widgets_;
};

}  // namespace sv

#endif  // SCENEVIEW_PARAM_WIDGET_HPP__
