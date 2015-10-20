// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_AXIS_ALIGNED_BOX_HPP__
#define SCENEVIEW_AXIS_ALIGNED_BOX_HPP__

#include <QVector3D>

namespace sceneview {

class AxisAlignedBox {
  public:
    AxisAlignedBox();

    AxisAlignedBox(const QVector3D& min, const QVector3D& max);

    void SetBounds(const QVector3D& min, const QVector3D& max);

    void IncludePoint(const QVector3D& point);

    void IncludeBox(const AxisAlignedBox& other);

    bool Valid() const;

    const QVector3D& Min() const { return min_; }

    const QVector3D& Max() const { return max_; }

    /**
     * Transforms and axis-aligns the corners of this box.
     */
    AxisAlignedBox Transformed(const QMatrix4x4& transform) const;

    /**
     * For debugging
     */
    QString ToString() const;

  private:
    QVector3D min_;
    QVector3D max_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_AXIS_ALIGNED_BOUNDING_BOX_HPP__
