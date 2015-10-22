#ifndef SCENEVIEW_DRAWABLE_HPP__
#define SCENEVIEW_DRAWABLE_HPP__

#include <memory>

#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>

namespace sv {

/**
 * Fundamental drawable unit.
 */
class Drawable {
  public:
    typedef std::shared_ptr<Drawable> Ptr;

  public:
    static Ptr Create(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material) {
      return Ptr(new Drawable(geometry, material));
    }

    const GeometryResource::Ptr& Geometry() { return geometry_; }

    const MaterialResource::Ptr& Material() { return material_; }

    /**
     * Called by the render engine just before rendering the geometry pair
     * referenced by this object.
     *
     * At the time this method is called, the material properties have been
     * loaded into the OpenGL state machine.  Specifically, the shader program
     * is bound, all shader uniform variables loaded, and OpenGL behavior
     * adjusted (e.g., depth test, line width, etc.)
     *
     * To override the builtin rendering engine behavior, you can create a
     * subclass of Drawable and override this method.
     *
     * If this method returns true (the default), then the render engine
     * draws the referenced geometry. If it returns false, then geometry
     * rendering is skipped.
     */
    virtual bool PreDraw() { return true; }

    /**
     * Called by the render engine just after rendering the geometry referenced
     * by this object.
     */
    virtual void PostDraw() {}

  private:
    Drawable(const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material) :
      geometry_(geometry), material_(material) {}

    GeometryResource::Ptr geometry_;
    MaterialResource::Ptr material_;
};

}  // namespace sv

#endif  // SCENEVIEW_DRAWABLE_HPP__
