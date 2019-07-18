// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_EXPANDER_WIDGET_HPP__
#define SCENEVIEW_EXPANDER_WIDGET_HPP__

#include <QWidget>

namespace sv {

/**
 * Hides or shows a contained widget. Based on the GtkExpander widget in GTK.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/expander_widget.hpp
 */
class ExpanderWidget : public QWidget {
  Q_OBJECT

  public:
    explicit ExpanderWidget(QWidget* parent = nullptr);

    virtual ~ExpanderWidget();

    void SetWidget(QWidget* widget);

    QSize sizeHint() const override;

    void SetTitle(const QString& title);
    QString Title() const;

    void SetExpanded(bool val);
    bool Expanded() const;

  private slots:
    void ToggleExpanded();

  private:
    struct Priv;

    Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_EXPANDER_WIDGET_HPP__
