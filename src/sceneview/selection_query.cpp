#include "selection_query.hpp"

#include <algorithm>
#include <deque>

#include "group_node.hpp"
#include "scene_node.hpp"

namespace sv {

struct SelectionQuery::Priv {
  Scene::Ptr scene;
};

SelectionQuery::SelectionQuery(const Scene::Ptr& scene) : p_(new Priv) {
  p_->scene = scene;
}

SelectionQuery::~SelectionQuery() { delete p_; }

bool SelectionQuery::Intersection(const AxisAlignedBox& box,
                                  const QVector3D& ray_start,
                                  const QVector3D& ray_dir, double* result) {
  const QVector3D& bmin = box.Min();
  const QVector3D& bmax = box.Max();
  const double inv_x = 1 / ray_dir.x();
  const double inv_y = 1 / ray_dir.y();
  const double inv_z = 1 / ray_dir.z();
  const double tx1 = (bmin.x() - ray_start.x()) * inv_x;
  const double tx2 = (bmax.x() - ray_start.x()) * inv_x;

  double tmin = std::min(tx1, tx2);
  double tmax = std::max(tx1, tx2);

  const double ty1 = (bmin.y() - ray_start.y()) * inv_y;
  const double ty2 = (bmax.y() - ray_start.y()) * inv_y;

  tmin = std::max(tmin, std::min(ty1, ty2));
  tmax = std::min(tmax, std::max(ty1, ty2));

  const double tz1 = (bmin.z() - ray_start.z()) * inv_z;
  const double tz2 = (bmax.z() - ray_start.z()) * inv_z;

  tmin = std::max(tmin, std::min(tz1, tz2));
  tmax = std::min(tmax, std::max(tz1, tz2));

  if (tmax >= tmin && tmax > 0) {
    *result = std::max(0.0, std::min(tmin, tmax));
    return true;
  }
  return false;
}

std::vector<QueryResult> SelectionQuery::CastRay(const int64_t selection_mask,
                                                 const QVector3D& start,
                                                 const QVector3D& dir) {
  std::vector<QueryResult> result;

  std::deque<SceneNode*> to_query = {p_->scene->Root()};

  while (!to_query.empty()) {
    SceneNode* node = to_query.front();
    to_query.pop_front();

    const AxisAlignedBox& node_box = node->WorldBoundingBox();
    double node_t;
    if (!Intersection(node_box, start, dir, &node_t)) {
      // No intersection. Move along.
      continue;
    }

    // Intersection with this node.
    // 1. Schedule its children for testing.
    if (node->NodeType() == SceneNodeType::kGroupNode) {
      GroupNode* group = static_cast<GroupNode*>(node);
      const std::vector<SceneNode*>& children = group->Children();
      to_query.insert(to_query.end(), children.begin(), children.end());
    }

    // 2. If the node passes the selection mask, then add it to the result list.
    if (node->GetSelectionMask() & selection_mask) {
      result.emplace_back(node, node_t);
    }
  }

  // Sort nodes by distance.
  std::sort(result.begin(), result.end(),
            [](const QueryResult& result_a, const QueryResult& result_b) {
              return result_a.distance < result_b.distance;
            });

  return result;
}

}  // namespace sv
