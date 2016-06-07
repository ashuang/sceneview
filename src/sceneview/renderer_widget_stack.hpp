// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_RENDERER_WIDGET_STACK
#define SCENEVIEW_RENDERER_WIDGET_STACK

#include <QDockWidget>

class QScrollArea;
class QVBoxLayout;

namespace sv {

class ExpanderWidget;
class Renderer;

/**
 * Contains and manages Renderer widgets.
 *
 * When an Renderer provides a widget by overriding Renderer::GetWidget(), the
 * Viewer class places them inside an RendererWidgetStack widget.
 *
 * Sceneview users generally will not have to interact with this class.
 *
 * @ingroup sv_gui
 * @headerfile sceneview/renderer_widget_stack.hpp
 */
class RendererWidgetStack : public QDockWidget {
  Q_OBJECT

  public:
    explicit RendererWidgetStack(QWidget* parent = nullptr);

    /**
     * Retrieve the expander widget for the corresponding renderer.
     *
     * If the renderer does not have a widget, then no expander widget is
     * created.
     *
     * @return an ExpanderWidget for the renderer, or nullptr if no widget exists.
     */
    ExpanderWidget* GetExpanderWidget(Renderer* renderer);

  public slots:
    void AddRendererWidget(Renderer* renderer);

  private:
    QScrollArea* area_;
    QWidget* container_;
    QVBoxLayout* layout_;
};

}  // namespace sv

#endif  // SCENEVIEW_RENDERER_WIDGET_STACK
