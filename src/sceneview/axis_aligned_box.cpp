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
