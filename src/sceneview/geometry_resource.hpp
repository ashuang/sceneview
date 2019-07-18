// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_GEOMETRY_RESOURCE_HPP__
#define SCENEVIEW_GEOMETRY_RESOURCE_HPP__

#include <cstdint>
#include <memory>
#include <vector>

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLBuffer>

#include <sceneview/axis_aligned_box.hpp>

namespace sv {

class Drawable;

/**
 * Geometry description to be used with GeometryResource.
 *
 * This struct contains the raw data for describing geometry.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/geometry_resource.hpp
 */
struct GeometryData {
  /**
   * Vertices of the geometry.
   */
  std::vector<QVector3D> vertices;

  /**
   * Normal vectors. This must be either empty or the same size as vertices.
   */
  std::vector<QVector3D> normals;

  /**
   * Diffuse color component. This must be either empty or the same size as
   * vertices.
   *
   * This field is used by the following builtin materials:
   * - sv::StockResources::kPerVertexColorLighting to provide the diffuse color
   *   component for a vertex
   * - sv::StockResources::kPerVertexColorNoLighting to provide the absolute
   *   color for a vertex.
   */
  std::vector<QVector4D> diffuse;

  /**
   * Specular color component. This must be either empty or the same size as
   * vertices.
   *
   * This field is used by the following builtin materials:
   * - sv::StockResources::kPerVertexColorLighting to provide the specular color
   *   component for a vertex
   */
  std::vector<QVector4D> specular;

  /**
   * Shininess component. This must be either empty or the same size as
   * vertices.
   *
   * This field is used by the following builtin materials:
   * - sv::StockResources::kPerVertexColorLighting to provide the shininess
   *   value for a vertex
   */
  std::vector<float> shininess;

  /**
   * Texture coordinates. This must be either empty or the same size as vertices.
   *
   * This field is used by the following builtin materials:
   * - sv::StockResouces::kTextureUniformColorNoLighting to provide texture
   *   coordinates.
   */
  std::vector<QVector2D> tex_coords_0;

  /**
   * Vertex indices. If specified, then the geometry is drawn using
   * glDrawElements(). If not, then the geometry is drawn with glDrawArrays().
   */
  std::vector<uint32_t> indices;

  /**
   * The OpenGL primitive type. Must be one of GL_TRIANGLES, GL_POINTS,
   * GL_LINES, etc ...
   */
  GLenum gl_mode;
};

/**
 * Geometry that can be rendered with glDrawArrays() or glDrawElements().
 *
 * A GeometryResource manages per-vertex attributes using a combination of
 * vertex buffer objects and index buffers.
 *
 * GeometryResource objects cannot be directly instantiated. Instead, use
 * ResourceManager or StockResources.
 *
 * Typically the data is loaded from a GeometryData() object.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/geometry_resource.hpp
 */
class GeometryResource {
  public:
    typedef std::shared_ptr<GeometryResource> Ptr;

    ~GeometryResource();

    /**
     * Loads the specified geometry into this resource.
     *
     * Automatically allocates buffers in graphics memory as needed.
     */
    void Load(const GeometryData& data);

    QOpenGLBuffer* VBO();

    QOpenGLBuffer* IndexBuffer();

    int VertexOffset() const;

    int NumVertices() const;

    int NormalOffset() const;

    int NumNormals() const;

    int DiffuseOffset() const;

    int NumDiffuse() const;

    int NumSpecular() const;

    int SpecularOffset() const;

    int NumShininess() const;

    int ShininessOffset() const;

    int TexCoords0Offset() const;

    int NumTexCoords0() const;

    int NumIndices() const;

    /**
     * Returns the type parameter to pass to glDrawElements()
     * Either GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
     */
    GLenum IndexType() const;

    /**
     * What kind of primitives are in this geometry (GL_POINTS, GL_LINE_STRIP,
     * ...)
     */
    GLenum GLMode() const;

    const AxisAlignedBox& BoundingBox() const;

  private:
    friend class ResourceManager;

    friend class Drawable;

    explicit GeometryResource(const QString& name);

    void AddListener(Drawable* drawable);

    void RemoveListener(Drawable* drawable);

    struct Priv;

    Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_GEOMETRY_RESOURCE_HPP__
