#ifndef SCENEVIEW_STOCK_SHAPE_RENDERER_HPP__
#define SCENEVIEW_STOCK_SHAPE_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

namespace vis_examples {

/**
 * Demonstrates using stock shapes, ParamWidget, and selection queries.
 *
 * Demonstrates:
 * - Instantiating stock shapes and inserting them into the scene.
 * - Using the result of a selection query to highlight a shape.
 * - Using a ParamWidget to control rendering behavior.
 */
class StockShapeRenderer : public sv::Renderer {
  Q_OBJECT

 public:
    StockShapeRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void RenderBegin() override;

    QWidget* GetWidget() override;

    QVariant SaveState() override;

    void LoadState(const QVariant& val) override;

    void NodeSelected(sv::SceneNode* node);

 private slots:
    void ParamChanged(const QString& name);

 private:
    std::vector<sv::DrawNode*> shapes_;

    sv::MaterialResource::Ptr material_;
    sv::MaterialResource::Ptr select_material_;

    sv::DrawNode* selected_node_;
    sv::MaterialResource::Ptr selected_orig_material_;

    QTime start_time_;
    double angle_;

    std::unique_ptr<sv::ParamWidget> widget_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_STOCK_SHAPE_RENDERER_HPP__
