#ifndef VIS_EXAMPLES_STOCK_SHAPE_SELECTOR_HPP__
#define VIS_EXAMPLES_STOCK_SHAPE_SELECTOR_HPP__

#include <sceneview/sceneview.hpp>

namespace vis_examples {

class StockShapeRenderer;

/**
 * Demonstrates using selection queries to interact with objects in the scene
 * graph.
 */
class StockShapeSelector : public QObject, public sv::InputHandler {
  Q_OBJECT

  public:
    StockShapeSelector(StockShapeRenderer* renderer,
        QObject* parent = nullptr);

    QString Name() const override { return "Selector"; }

    void MousePressEvent(QMouseEvent* event) override;

  private:
    StockShapeRenderer* renderer_;
};

}  // namespace vis_examples

#endif  // VIS_EXAMPLES_STOCK_SHAPE_SELECTOR_HPP__
