// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_STOCK_RESOURCES_HPP_
#define SCENEVIEW_STOCK_RESOURCES_HPP_

#include <memory>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/resource_manager.hpp>

namespace sceneview {

/**
 * Functions to generate stock resources.
 */
class StockResources {
  public:
    enum StockShaderId {
      kUniformColorNoLighting,
      /**
       * Has the following uniform properties:
       *   "ambient": 4 floats
       *   "diffuse": 4 floats
       *   "specular": 4 floats
       *   "shininess": 1 float
       */
      kUniformColorLighting,
      kPerVertexColorLighting,
      kBillboardTextured,
      kBillboardUniformColor
    };

  public:
    explicit StockResources(const ResourceManager::Ptr& resources);

    GeometryResource::Ptr Cone();

    GeometryResource::Ptr Cube();

    GeometryResource::Ptr Cylinder();

    GeometryResource::Ptr Sphere();

    /**
     * Stock unit axes, with:
     * - X axis colored red
     * - Y axis colored green
     * - Z axis colored blue
     *
     * This is returned as a GeometryMaterialPair since the geometry (the axes)
     * is paired with a material (the color properties).
     *
     * The returned object can be directly added to a MeshNode. e.g.,:
     * @code
     *   StockResources stock(resources);
     *   MeshNode* mesh = scene->MakeMesh(parent);
     *   mesh->Add(stock.UnitAxes());
     * @endcode
     */
    GeometryMaterialPair UnitAxes();

    ShaderResource::Ptr Shader(StockShaderId id);

    /**
     * Convenience method that makes a new material attached to a stock shader.
     *
     * Calling this method is equivalent to:
     * @code
     *   resources->MakeMaterial(stock_resources.Shader(id));
     * @endcode
     */
    MaterialResource::Ptr NewMaterial(StockShaderId id);

    /**
     * Generate a cube.
     *
     * - Each dimension is unit length.
     * - The cube is centered at the origin in model space.
     */
    static GeometryData CubeData();

    /**
     * Generate a sphere of diameter 1 centered at the origin.
     */
    static GeometryData SphereData();

    /**
     * Generate a cone that fits in a unit cube centered on the origin.
     *
     * - Tip is at Z = +0.5
     * - Base is at Z = -0.5
     */
    static GeometryData ConeData();

    /**
     * Generate a cylinder that fits in a unit cube centered on the origin.
     *
     * - Diameter 1 and length 1.
     * - The axis of revolution is the Z axis.
     * - Centered on the origin.
     */
    static GeometryData CylinderData();

    /**
     * Generate a set of unit axes.
     */
    static GeometryData UnitAxesData();

  private:
    ResourceManager::Ptr resources_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_STOCK_RESOURCES_HPP_
