// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_AXIS_ALIGNED_BOX_HPP__
#define SCENEVIEW_AXIS_ALIGNED_BOX_HPP__

#include <QVector3D>

namespace sv {

/**
 * An axis-aligned box typically used for bounding box calculations.
 *
 * @ingroup sv_scenegraph
 * @headerfile sceneview/axis_aligned_box.hpp
 */
class AxisAlignedBox {
  public:
    /**
     * Constructs an invalid box.
     *
     * The box can be made valid by including at least one point (e.g., by
     * calling IncludePoint() with a finite value).
     */
    AxisAlignedBox();

    /**
     * Constructs a box with the specified extents.
     */
    AxisAlignedBox(const QVector3D& min, const QVector3D& max);

    /**
     * Manually set the box extents.
     */
    void SetBounds(const QVector3D& min, const QVector3D& max);

    /**
     * Modifies the box to contain the specified point.
     */
    void IncludePoint(const QVector3D& point);

    /**
     * Modifies the box to fully contain the specified other box.
     */
    void IncludeBox(const AxisAlignedBox& other);

    /**
     * Check if the box is valid or not. Boxes with negative volume are
     * considered invalid.
     */
    bool Valid() const;

    /**
     * Returns the box corner with lowest coordinates.
     */
    const QVector3D& Min() const { return min_; }

    /**
     * Returns the box corner with greatest coordinates.
     */
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

}  // namespace sv

#endif  // SCENEVIEW_AXIS_ALIGNED_BOUNDING_BOX_HPP__
