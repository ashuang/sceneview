#ifndef SCENEVIEW_STOCK_SHAPES_RENDERER_HPP__
#define SCENEVIEW_STOCK_SHAPES_RENDERER_HPP__

#include <QTime>

#include <sceneview/sceneview.hpp>

namespace vis_examples {

class StockShapesRenderer : public sv::Renderer {
  Q_OBJECT

 public:
    StockShapesRenderer(const QString& name, QObject* parent = 0);

    void InitializeGL() override;

    void RenderBegin() override;

    QWidget* GetWidget() override;

    QVariant SaveState() override;

    void LoadState(const QVariant& val) override;

 private slots:
    void ParamChanged(const QString& name);

 private:
    sv::MaterialResource::Ptr material_;
    std::vector<sv::MeshNode*> shapes_;

    QTime start_time_;
    double angle_;

    std::unique_ptr<sv::ParamWidget> widget_;
};

}  // namespace vis_examples

#endif  // SCENEVIEW_STOCK_SHAPES_RENDERER_HPP__
