#ifndef SCENEVIEW_PLANE_HPP__
#define SCENEVIEW_PLANE_HPP__

#include <QVector3D>

namespace sv {

class Plane {
  public:
    Plane();
    Plane(double a, double b, double c, double d);
    Plane(const QVector3D& normal, double d);

    /**
     * Create a plane from three points.
     *
     * The plane normal vector is taken as the cross product of (p2 - p1) with
     * (p3 - p1).
     */
    static Plane FromThreePoints(const QVector3D& p1, const QVector3D& p2,
        const QVector3D& p3);

    double SignedDistance(const QVector3D& point) const;

    const QVector3D& Normal() const { return normal_; }

    double A() const { return normal_.x(); }

    double B() const { return normal_.y(); }

    double C() const { return normal_.z(); }

    double D() const { return d_; }

  private:
    QVector3D normal_;
    double d_;
};

}  // namespace sv

#endif  // SCENEVIEW_PLANE_HPP__
