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

    QOpenGLBuffer* VBO() { return &vbo_; }

    QOpenGLBuffer* IndexBuffer();

    int VertexOffset() const { return vertex_offset_; }

    int NumVertices() const { return num_vertices_; }

    int NormalOffset() const { return normal_offset_; }

    int NumNormals() const { return num_normals_; }

    int DiffuseOffset() const { return diffuse_offset_; }

    int NumDiffuse() const { return num_diffuse_; }

    int NumSpecular() const { return num_specular_; }

    int SpecularOffset() const { return specular_offset_; }

    int NumShininess() const { return num_shininess_; }

    int ShininessOffset() const { return shininess_offset_; }

    int TexCoords0Offset() const { return tex_coords_0_offset_; }

    int NumTexCoords0() const { return num_tex_coords_0_; }

    int NumIndices() const { return num_indices_; }

    /**
     * Returns the type parameter to pass to glDrawElements()
     * Either GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
     */
    GLenum IndexType() const { return index_type_; }

    /**
     * What kind of primitives are in this geometry (GL_POINTS, GL_LINE_STRIP,
     * ...)
     */
    GLenum GLMode() const { return gl_mode_; }

    const AxisAlignedBox& BoundingBox() const { return bounding_box_; }

  private:
    friend class ResourceManager;

    friend class Drawable;

    explicit GeometryResource(const QString& name);

    void AddListener(Drawable* drawable);

    void RemoveListener(Drawable* drawable);

    const QString name_;

    // Vertex buffer to hold the data in graphics memory
    bool created_vbo_;
    QOpenGLBuffer vbo_;
    QOpenGLBuffer index_buffer_;

    int vertex_offset_;
    int normal_offset_;
    int diffuse_offset_;
    int specular_offset_;
    int shininess_offset_;
    int tex_coords_0_offset_;

    int num_vertices_;
    int num_normals_;
    int num_diffuse_;
    int num_specular_;
    int num_shininess_;
    int num_tex_coords_0_;

    int num_indices_;

    GLenum gl_mode_;
    GLenum index_type_;

    AxisAlignedBox bounding_box_;

    std::vector<Drawable*> listeners_;
};

}  // namespace sv

#endif  // SCENEVIEW_GEOMETRY_RESOURCE_HPP__
