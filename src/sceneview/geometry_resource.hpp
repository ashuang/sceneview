#ifndef SCENEVIEW_GEOMETRY_RESOURCE_HPP__
#define SCENEVIEW_GEOMETRY_RESOURCE_HPP__

#include <cstdint>
#include <memory>

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLBuffer>

#include <sceneview/axis_aligned_box.hpp>

namespace sceneview {

struct GeometryData {
  std::vector<QVector3D> vertices;

  std::vector<QVector3D> normals;

  std::vector<QVector4D> diffuse;

  std::vector<QVector4D> specular;

  std::vector<float> shininess;

  // texture coordinates
  std::vector<QVector2D> tex_coords_0;

  std::vector<uint32_t> indices;

  // GL_TRIANGLES, GL_POINTS, etc ...
  GLenum gl_mode;
};

/**
 * Geometry that can be rendered with glDrawArrays() or glDrawElements().
 *
 * A GeometryResource manages per-vertex attributes using a combination of
 * vertex buffer objects and index buffers.
 *
 * Typically the data is loaded from a GeometryData() object.
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

    const AxisAlignedBox BoundingBox() const { return bounding_box_; }

  private:
    friend class ResourceManager;

    GeometryResource(const QString& name);

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
};

}  // namespace sceneview

#endif  // SCENEVIEW_GEOMETRY_RESOURCE_HPP__
