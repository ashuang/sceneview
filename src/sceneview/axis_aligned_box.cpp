// Copyright [2015] Albert Huang

#include "sceneview/axis_aligned_box.hpp"

#include <algorithm>
#include <limits>

#include <QMatrix4x4>
#include <QString>

namespace sv {

AxisAlignedBox::AxisAlignedBox() :
  min_(std::numeric_limits<double>::max(),
       std::numeric_limits<double>::max(),
       std::numeric_limits<double>::max()),
  max_(std::numeric_limits<double>::lowest(),
       std::numeric_limits<double>::lowest(),
       std::numeric_limits<double>::lowest()) {}
AxisAlignedBox::AxisAlignedBox(const QVector3D& min,
    const QVector3D& max) {
  SetBounds(min, max);
}

void AxisAlignedBox::SetBounds(const QVector3D& min,
    const QVector3D& max) {
  min_ = min;
  max_ = max;
}

void AxisAlignedBox::IncludePoint(const QVector3D& point) {
  min_.setX(std::min(min_.x(), point.x()));
  min_.setY(std::min(min_.y(), point.y()));
  min_.setZ(std::min(min_.z(), point.z()));

  max_.setX(std::max(max_.x(), point.x()));
  max_.setY(std::max(max_.y(), point.y()));
  max_.setZ(std::max(max_.z(), point.z()));
}

void AxisAlignedBox::IncludeBox(const AxisAlignedBox& other) {
  IncludePoint(other.Min());
  IncludePoint(other.Max());
}

bool AxisAlignedBox::Valid() const {
  return min_.x() <= max_.x() &&
         min_.y() <= max_.y() &&
         min_.z() <= max_.z();
}

AxisAlignedBox AxisAlignedBox::Transformed(const QMatrix4x4& transform) const {
  const double x0 = min_.x();
  const double y0 = min_.y();
  const double z0 = min_.z();

  const double x1 = max_.x();
  const double y1 = max_.y();
  const double z1 = max_.z();

  AxisAlignedBox result;
  result.IncludePoint(transform * min_);
  result.IncludePoint(transform * max_);
  result.IncludePoint(transform * QVector3D(x0, y1, z0));
  result.IncludePoint(transform * QVector3D(x1, y1, z0));
  result.IncludePoint(transform * QVector3D(x1, y0, z0));
  result.IncludePoint(transform * QVector3D(x0, y0, z1));
  result.IncludePoint(transform * QVector3D(x0, y1, z1));
  result.IncludePoint(transform * QVector3D(x1, y0, z1));
  return result;
}

bool Isect1d(double a0, double a1, double b0, double b1,
    double* r0, double* r1) {
  if (a1 < b0 || b1 < a0) {
    return false;
  }
  *r1 = (a1 < b1) ? a1 : b1;
  *r0 = (a0 < b0) ? b0 : a0;
  return true;
}

bool AxisAlignedBox::Intersects(const AxisAlignedBox& other) const {
  if (!Valid() || !other.Valid()) {
    throw std::invalid_argument("Can't check intersection with an invalid bounding box");
  }
  return !(max_.x() < other.min_.x() || min_.x() > other.max_.x() ||
           max_.y() < other.min_.x() || min_.x() > other.max_.x() ||
           max_.z() < other.min_.z() || min_.z() > other.max_.z());
}

AxisAlignedBox AxisAlignedBox::Intersection(const AxisAlignedBox& other) const {
  if (!Valid() || !other.Valid()) {
    return AxisAlignedBox();
  }
  double x0;
  double x1;
  double y0;
  double y1;
  double z0;
  double z1;
  if (!Isect1d(min_.x(), max_.x(), other.min_.x(), other.max_.x(), &x0, &x1)) {
    return AxisAlignedBox();
  }
  if (!Isect1d(min_.y(), max_.y(), other.min_.y(), other.max_.y(), &y0, &y1)) {
    return AxisAlignedBox();
  }
  if (!Isect1d(min_.z(), max_.z(), other.min_.z(), other.max_.z(), &z0, &z1)) {
    return AxisAlignedBox();
  }
  return AxisAlignedBox(QVector3D(x0, y0, z0), QVector3D(x1, y1, z1));
}

bool AxisAlignedBox::operator==(const AxisAlignedBox& other) const {
  return min_ == other.min_ && max_ == other.max_;
}

QString AxisAlignedBox::ToString() const {
  if (!Valid()) {
    return "invalid";
  }
  return QString("<%1, %2, %3> - <%4, %5, %6>")
    .arg(min_.x(), 3)
    .arg(min_.y(), 3)
    .arg(min_.z(), 3)
    .arg(max_.x(), 3)
    .arg(max_.y(), 3)
    .arg(max_.z(), 3);
}

}  // namespace sv
