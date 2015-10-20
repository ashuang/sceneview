// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_STOCK_RESOURCES_HPP_
#define SCENEVIEW_STOCK_RESOURCES_HPP_

#include <memory>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/resource_manager.hpp>

namespace sv {

/**
 * String constant parameter name used by some stock shaders.
 */
extern const QString kColor;

/**
 * String constant parameter name used by some stock shaders.
 */
extern const QString kDiffuse;

/**
 * String constant parameter name used by some stock shaders.
 */
extern const QString kSpecular;

/**
 * String constant parameter name used by some stock shaders.
 */
extern const QString kShininess;

/**
 * Functions to generate stock resources.
 */
class StockResources {
  public:
    enum StockShaderId {
      /**
       * Color is identical for all vertices, and there is no lighting.
       *
       * To specify the color, use the MaterialResource parameter
       * sv::kColor.
       *
       * For example:
       * @code
       *  ResourceManager::Ptr resources = GetResourceManager();
       *  Scene::Ptr scene = GetScene();
       *  StockResources stock(resources);
       *
       *  MaterialResource::Ptr material =
       *      stock.NewMaterial(StockResources::kUniformColorNoLighting);
       *  material->SetParam(sv::kColor, 0.0, 1.0, 0.0, 1.0);
       *
       *  scene->MakeMesh(scene->Root(), stock.Cube(), material);
       * @endcode
       */
      kUniformColorNoLighting,
      /**
       * Uses the stock lighting model with identical colors for all vertices.
       * To specify the colors, use the following MaterialResource parameters:
       *   - sv::kDiffuse : 4 floats
       *   - sv::kSpecular : 4 floats
       *   - sv::kShininess : 1 float
       *
       *  All parameters are optional, and if not specified default to 0.0.
       *
       *  For example:
       *  @code
       *  ResourceManager::Ptr resources = GetResourceManager();
       *  Scene::Ptr scene = GetScene();
       *  StockResources stock(resources);
       *
       *  MaterialResource::Ptr material =
       *      stock.NewMaterial(StockResources::kUniformColorLighting);
       *  material->SetParam(sv::kDiffuse, 0.9, 0.0, 0.0, 1.0);
       *
       *  scene->MakeMesh(scene->Root(), stock.Cube(), material);
       *  @endcode
       */
      kUniformColorLighting,
      /**
       * Color is specified on a per-vertex basis, with no lighting
       * calculations.
       *
       * To specify the vertex color, use the GeometryData::diffuse field.
       */
      kPerVertexColorNoLighting,
      /**
       * Uses the stock lighting model with colors specified on a per-vertex
       * basis. To specify a vertex color, use the diffuse, specular, and
       * shininess fields of the GeometryData structure. For example:
       *
       * @code
       * GeometryData gdata;
       * gdata.gl_mode = GL_TRIANGLES;
       * gdata.vertices.emplace_back(0, 0, 0);
       * gdata.vertices.emplace_back(1, 0, 0);
       * gdata.vertices.emplace_back(0, 1, 0);
       * gdata.normals.emplace_back(0, 0, 1);
       * gdata.normals.emplace_back(0, 0, 1);
       * gdata.normals.emplace_back(0, 0, 1);
       * gdata.diffuse.emplace_back(1, 0, 0);
       * gdata.diffuse.emplace_back(0, 1, 0);
       * gdata.diffuse.emplace_back(0, 0, 1);
       * gdata.specular.emplace_back(1, 0, 0);
       * gdata.specular.emplace_back(0, 1, 0);
       * gdata.specular.emplace_back(0, 0, 1);
       * gdata.shininess.push_back(16);
       * gdata.shininess.push_back(16);
       * gdata.shininess.push_back(16);
       *
       * ResourceManager::Ptr resources = GetResourceManager();
       * GeometryResource::Ptr geometry = resources->MakeGeometry();
       * geometry->Load(gdata);
       *
       * StockResources stock(resources);
       * MaterialResource::Ptr material =
       *     stock.NewMaterial(kPerVertexColorLighting);
       *
       * Scene::Ptr scene = GetScene();
       * scene->MakeMesh(scene->Root(), geometry, material);
       * @endcode
       */
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

}  // namespace sv

#endif  // SCENEVIEW_STOCK_RESOURCES_HPP_
