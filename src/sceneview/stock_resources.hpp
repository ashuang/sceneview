// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_STOCK_RESOURCES_HPP_
#define SCENEVIEW_STOCK_RESOURCES_HPP_

#include <memory>
#include <sceneview/drawable.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/resource_manager.hpp>

namespace sv {

/**
 * @ingroup sv_resources
 * @{
 */

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
 * String constant parameter name used by the texture stock shaders.
 */
extern const QString kTexture0;

/**
 * Functions to generate stock resources.
 *
 *
 * ## Stock geometry resources
 * The following methods can be used to retrieve stock geometry resources:
 * - Cone()
 * - Cube()
 * - Cylinder()
 * - Sphere()
 * - UnitAxes()
 *
 * When using these methods, each stock geometry resource is reused if it's
 * already been created. They are maintained with a reference counted pointer,
 * so when the last reference to the goes out of scope, the geometry is
 * destroyed.
 *
 * e.g.,:
 * @code
 * ResourceManager::Ptr resources = GetResourceManager();
 * StockResources stock(resources);
 *
 * GeometryResource::Ptr cone1 = stock.Cone();
 * GeometryResource::Ptr cone2 = stock.Cone();
 * // At this point, cone1 == cone2.
 *
 * cone1.reset();
 * cone2.reset();
 *
 * // Now that the last reference to the cone geometry has gone out of scope,
 * // the actual resources used by the cone geometry are released and
 * // destroyed.
 * @endcode
 *
 * ## Stock geometry data
 * Similar to the methods for retrieving stock geometry resources, there are
 * also static methods for generating the raw geometry data used by the
 * resources. These methods might be useful if you're bypassing the Sceneview
 * rendering engine, or you want to modify the data somehow before using it.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/stock_resources.hpp
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
     *  scene->MakeDrawNode(scene->Root(), stock.Cube(), material);
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
     *  scene->MakeDrawNode(scene->Root(), stock.Cube(), material);
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
     *     stock.NewMaterial(StockResources::kPerVertexColorLighting);
     *
     * Scene::Ptr scene = GetScene();
     * scene->MakeDrawNode(scene->Root(), geometry, material);
     * @endcode
     */
    kPerVertexColorLighting,
    /**
     * Like kUniformColorNoLighting with the addition of a texture map.
     *
     * Color is calculated by multiplying the uniform color by the texture
     * color.
     *
     * To use this shader with a material:
     * - set the uniform color via MaterialResource::SetParam(sv::kColor, r, g,
     * b, a);
     * - bind the texture via MaterialResource::AddTexture(sv::kTexture0,
     * texture_);
     *
     * For example:
     *
     * @code
     * GeometryData gdata;
     * gdata.gl_mode = GL_TRIANGLES;
     * gdata.vertices = {
     *   { 0, 0, 0 },
     *   { 0, 1, 0 },
     *   { 1, 1, 0 },
     *   { 1, 0, 0 }
     * };
     * gdata.tex_coords_0 = {
     *   { 0, 0 },
     *   { 1, 0 },
     *   { 1, 1 },
     *   { 0, 1 }
     * };
     * gdata.indices = { 0, 1, 2, 0, 2, 3 };
     *
     * ResourceManager::Ptr resources = GetResourceManager();
     * GeometryResource::Ptr geometry = resources->MakeGeometry();
     * geometry->Load(gdata);
     *
     * StockResources stock(resources);
     * MaterialResource::Ptr material =
     *     stock.NewMaterial(StockResources::kTextureUniformColorNoLighting);
     * material->SetParam(sv::kColor, 1.0, 1.0, 1.0, 1.0);
     *
     * QImage image(400, 400, QImage::Format_RGB888);
     * image.fill(QColor(255, 0, 0));
     * QOpenGLTexture* texture = new QOpenGLTexture(image);
     * material->AddTexture(sv::kTexture, texture);
     *
     * Scene::Ptr scene = GetScene();
     * scene->MakeDrawNode(scene->Root(), geometry, material);
     * @endcode
     */
    kTextureUniformColorNoLighting,
    /**
     * Like kUniformColorLighting with the addition of a texture map.
     */
    kTextureUniformColorLighting,
    kBillboardTextured,
    kBillboardUniformColor
  };

 public:
  /**
   * Constructor.
   */
  explicit StockResources(const ResourceManager::Ptr& resources);

  ~StockResources();

  /**
   * Retrieves the stock cone geometry resource.
   *
   * - Tip is at Z = +0.5
   * - Base is at Z = -0.5
   * - The cone fits in a unit cube centered on the origin.
   */
  GeometryResource::Ptr Cone();

  /**
   * Retrieves the stock cone geometry resource.
   *
   * - Each dimension is unit length.
   * - The cube is centered at the origin in model space.
   */
  GeometryResource::Ptr Cube();

  /**
   * Retrieve the stock cylinder geometry resource.
   *
   * - Diameter 1 and length 1.
   * - The axis of revolution is the Z axis.
   * - Centered on the origin.
   */
  GeometryResource::Ptr Cylinder();

  /**
   * Retrieve the stock sphere geometry resource.
   *
   * - Diameter 1
   * - Centered on the origin.
   */
  GeometryResource::Ptr Sphere();

  /**
   * Stock unit axes, with:
   * - X axis colored red
   * - Y axis colored green
   * - Z axis colored blue
   *
   * This is returned as a Drawable since the geometry (the axes)
   * is paired with a material (the color properties).
   *
   * The returned object can be directly added to a DrawNode. e.g.,:
   * @code
   *   StockResources stock(resources);
   *   DrawNode* draw_node = scene->MakeDrawNode(parent);
   *   draw_node->Add(stock.UnitAxes());
   * @endcode
   */
  Drawable::Ptr UnitAxes();

  /**
   * Retrieve the shader resource corresponding to the specified stock
   * shader.
   */
  ShaderResource::Ptr Shader(StockShaderId id);

  /**
   * Convenience method that makes a new material attached to a stock shader.
   *
   * Calling this method is equivalent to:
   * @code
   *   resources->MakeMaterial(stock_resources.Shader(id));
   * @endcode
   *
   * For information on how to adjust the material parameters, see the
   * documentation for the specified stock shader id.
   */
  MaterialResource::Ptr NewMaterial(StockShaderId id);

  /**
   * Generate geometry data for a cone.
   *
   * - Tip is at Z = +0.5
   * - Base is at Z = -0.5
   * - The cone fits in a unit cube centered on the origin.
   */
  static GeometryData ConeData();

  /**
   * Generate geometry data for a unit cube.
   *
   * - Each dimension is unit length.
   * - The cube is centered at the origin in model space.
   */
  static GeometryData CubeData();

  /**
   * Generate geometry data for a cylinder that fits in a unit cube centered
   * on the origin.
   *
   * - Diameter 1 and length 1.
   * - The axis of revolution is the Z axis.
   * - Centered on the origin.
   */
  static GeometryData CylinderData();

  /**
   * Generate geometry data for a sphere of diameter 1 centered at the
   * origin.
   */
  static GeometryData SphereData();

  /**
   * Generate geometry data for a set of unit axes.
   */
  static GeometryData UnitAxesData();

 private:
  struct Priv;
  Priv* p_;
};

/**
 * @}
 */

}  // namespace sv

#endif  // SCENEVIEW_STOCK_RESOURCES_HPP_
