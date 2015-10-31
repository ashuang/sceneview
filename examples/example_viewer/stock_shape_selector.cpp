#include "stock_shape_selector.hpp"
#include "stock_shape_renderer.hpp"

using sv::CameraNode;
using sv::Viewport;
using sv::SelectionQuery;
using sv::QueryResult;

namespace vis_examples {

StockShapeSelector::StockShapeSelector(StockShapeRenderer* renderer,
    QObject* parent) :
  QObject(parent),
  renderer_(renderer) {
}

void StockShapeSelector::MousePressEvent(QMouseEvent* event) {
  // Convert the mouse coordinates into a world-space ray.
  Viewport* viewport = renderer_->GetViewport();
  CameraNode* camera = viewport->GetCamera();
  const QVector3D dir = camera->Unproject(event->x(), event->y()).normalized();
  const QVector3D start = camera->Translation();

  // Cast the ray into the scene to select an object. This query is performed
  // using axis-aligned bounding boxes, which are fast and generally
  // sufficient. The bounding boxes are automatically computed from the scene
  // graph geometry, so we get the tests "for free".
  //
  // If we wanted more precise ray casting, then we could cull the results of
  // the query with a more exact method (e.g., based on a custom collision
  // volume).
  const int64_t selection_mask = 0x1;
  SelectionQuery query(renderer_->GetScene());
  std::vector<QueryResult> qresult = query.CastRay(selection_mask, start, dir);

  if (qresult.empty()) {
    return;
  }

  // Found an object. Pass it on to StockShapeRenderer
  QueryResult& first_result = qresult.front();
  renderer_->NodeSelected(first_result.node);
}

}  // namespace vis_examples
