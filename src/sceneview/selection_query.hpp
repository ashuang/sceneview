#ifndef SCENEVIEW_SELECTION_QUERY_HPP__
#define SCENEVIEW_SELECTION_QUERY_HPP__

#include <QVector3D>

#include <sceneview/scene.hpp>

namespace sv {

struct QueryResult {
  QueryResult(SceneNode* node, double distance)
      : node(node), distance(distance) {}

  SceneNode* node;
  double distance;
};

/**
 * Use to select objects in the scene.
 *
 * ## Bounding boxes
 * During a selection query, only the axis-aligned bounding boxes for a node
 * are considered. This may result in false positives, and it is up to the user
 * to perform more detailed selection test.
 *
 * ## Selection mask
 * Each node in the scene is assigned a selection mask, which can be used by
 * the user to determine what nodes are candidates for selection during a
 * query. During a query, the query selection mask is bitwise-ANDed with each
 * node's mask, and only nodes with a nonzero match are considered. You can use
 * this to implement different selection categories.
 *
 * By default, a node's selection mask is 0, which means that no nodes are
 * selectable by default.
 */
class SelectionQuery {
 public:
  SelectionQuery(const Scene::Ptr& scene);

  virtual ~SelectionQuery();

  /**
   * Perform a ray cast selection query.
   *
   * @param selection_mask the selection mask to use when considering nodes.
   * @param start the ray starting point, in world coordinates.
   * @param dir the ray direction, in world coordinates.  Does not need to be
   *            normalized.
   *
   * @return a vector of matching nodes, sorted in ascending order of
   * distance along the ray (i.e., the closest matching nodes are first).
   */
  std::vector<QueryResult> CastRay(const int64_t selection_mask,
                                   const QVector3D& start,
                                   const QVector3D& dir);

  static bool Intersection(const AxisAlignedBox& box,
                           const QVector3D& ray_start, const QVector3D& ray_dir,
                           double* result);

 private:
  struct Priv;
  Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_SELECTION_QUERY_HPP__
