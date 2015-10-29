#include "plane.hpp"

namespace sv {

Plane::Plane() : normal_(0, 0, 0), d_(0) {}

Plane::Plane(double a, double b, double c, double d) {
  const QVector3D normal(a, b, c);
  const double mag = normal.length();
  if (mag < 1e-9) {
    normal_ = normal;
    d_ = d;
  } else {
    normal_ = normal / mag;
    d_ = d / mag;
  }
}

Plane::Plane(const QVector3D& normal, double d) :
  Plane(normal.x(), normal.y(), normal.z(), d) {}

Plane Plane::FromThreePoints(const QVector3D& p1, const QVector3D& p2,
        const QVector3D& p3) {
  const QVector3D p3_p1 = p3 - p1;
  const QVector3D p2_p1 = p2 - p1;
  const QVector3D normal = QVector3D::crossProduct(p2_p1, p3_p1).normalized();
  return Plane(normal, -QVector3D::dotProduct(normal, p1));
}

double Plane::SignedDistance(const QVector3D& point) const {
  return QVector3D::dotProduct(normal_, point) + d_;
}

}  // namespace sv
