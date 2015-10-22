#ifndef SCENEVIEW_DRAWABLE_HPP__
#define SCENEVIEW_DRAWABLE_HPP__

#include <memory>

#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>

namespace sv {

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

    virtual bool PreDraw() { return true; }

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
