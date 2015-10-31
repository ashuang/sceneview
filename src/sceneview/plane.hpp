#ifndef SCENEVIEW_PLANE_HPP__
#define SCENEVIEW_PLANE_HPP__

#include <QVector3D>

namespace sv {

/**
 * A 3D plane.
 *
 * The plane is represented interally as the parameters
 * @f[
 * (a, b, c, d)
 * @f]
 *
 * with the plane's normal vector given as
 * @f$ \mathbf{n} = (a, b, c) @f$ and @f$ || \mathbf{n} || = 1 @f$.
 *
 * All points @f$(x, y, z)@f$ on the plane satisfy:
 * @f[
 * a x + b y + c z + d = 0
 * @f]
 *
 * @headerfile sceneview/plane.hpp
 */
class Plane {
  public:
    /**
     * Construct an invalid plane with zero coefficients.
     */
    Plane();

    /**
     * Construct a plane with the specified parameters.
     *
     * Interally, the parameters are scaled so that @f$|| (a, b, c)|| = 1@f$
     */
    Plane(float a, float b, float c, float d);

    /**
     * Construct a plane with the specified parameters.
     *
     * Interally, the parameters are scaled so that @f$|| (a, b, c)|| = 1@f$
     */
    Plane(const QVector3D& normal, float d);

    /**
     * Create a plane from three points.
     *
     * The plane normal vector is taken as the cross product of (p2 - p1) with
     * (p3 - p1):
     *
     * @f[
     * \mathbf{n} = (\mathbf{p_2} - \mathbf{p_1}) \times (\mathbf{p_3} - \mathbf{p_1})
     * @f]
     *
     * The normal vector is then scaled to be unit length, and a suitable value
     * for d is chosen automatically.
     *
     */
    static Plane FromThreePoints(const QVector3D& p1, const QVector3D& p2,
        const QVector3D& p3);

    /**
     * Computes the signed distance of a point from the plane. The point has a
     * positive distance from the plane if the vector from the plane to the
     * point has a positive dot product with the plane's normal vector.
     *
     * Specifically, this method computes:
     * @f[
     * \mathbf{n} \cdot \mathbf{point} + d
     * @f]
     *
     * This method can be used as a half-plane test to determine which side of
     * the plane the point lies on. This is particularly useful for view
     * frustum culling.
     */
    float SignedDistance(const QVector3D& point) const;

    /**
     * Returns the plane's normal vector @f$ \mathbf{n} = (a, b, c) @f$
     */
    const QVector3D& Normal() const { return normal_; }

    /**
     * Returns the plane's d parameter.
     */
    float D() const { return d_; }

  private:
    QVector3D normal_;
    float d_;
};

}  // namespace sv

#endif  // SCENEVIEW_PLANE_HPP__
