// Copyright [2015] Albert Huang

#include "sceneview/geometry_resource.hpp"

#include <vector>

#include "drawable.hpp"

#if 0
#define dbg(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define dbg(...)
#endif

namespace sv {

struct GeometryResource::Priv {
  QString name;

  // Vertex buffer to hold the data in graphics memory
  bool created_vbo;
  QOpenGLBuffer vbo;
  QOpenGLBuffer index_buffer;

  int vertex_offset;
  int normal_offset;
  int diffuse_offset;
  int specular_offset;
  int shininess_offset;
  int tex_coords_0_offset;

  int num_vertices;
  int num_normals;
  int num_diffuse;
  int num_specular;
  int num_shininess;
  int num_tex_coords_0;

  int num_indices;

  GLenum gl_mode;
  GLenum index_type;

  AxisAlignedBox bounding_box;

  std::vector<Drawable*> listeners;
};

GeometryResource::GeometryResource(const QString& name) : p_(new Priv()) {
  p_->name = name;
  p_->created_vbo = false;
  p_->index_buffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  p_->vertex_offset = 0;
  p_->normal_offset = 0;
  p_->diffuse_offset = 0;
  p_->specular_offset = 0;
  p_->shininess_offset = 0;
  p_->tex_coords_0_offset = 0;
  p_->num_vertices = 0;
  p_->num_normals = 0;
  p_->num_diffuse = 0;
  p_->num_specular = 0;
  p_->num_shininess = 0;
  p_->num_tex_coords_0 = 0;
  p_->num_indices = 0;
  p_->gl_mode = 0;
  p_->index_type = GL_UNSIGNED_INT;
}

GeometryResource::~GeometryResource() {
  dbg("destroying geometry resource %s\n", p_->name.c_str());
  if (p_->created_vbo) {
    p_->vbo.destroy();
  }
  if (p_->num_indices) {
    p_->index_buffer.destroy();
  }
  delete p_;
}

void GeometryResource::Load(const GeometryData& data) {
  if (!p_->created_vbo) {
    p_->vbo.create();
    p_->created_vbo = true;
  }
  p_->vbo.bind();

  p_->vertex_offset = 0;
  p_->normal_offset = 0;
  p_->diffuse_offset = 0;
  p_->specular_offset = 0;
  p_->shininess_offset = 0;
  p_->tex_coords_0_offset = 0;

  const int num_vertices = data.vertices.size();
  const int num_normals = data.normals.size();
  const int num_diffuse = data.diffuse.size();
  const int num_specular = data.specular.size();
  const int num_shininess = data.shininess.size();
  const int num_tex_coords_0 = data.tex_coords_0.size();

  // check inputs
  if (num_vertices != num_normals && (num_normals != 0)) {
    throw std::invalid_argument("#vertices != #normals");
  }
  if (num_vertices != num_diffuse && (num_diffuse != 0)) {
    throw std::invalid_argument("#vertices != #diffuse");
  }
  if (num_vertices != num_specular && (num_specular != 0)) {
    throw std::invalid_argument("#vertices != #specular");
  }
  if (num_vertices != num_shininess && (num_shininess != 0)) {
    throw std::invalid_argument("#vertices != #shininess");
  }
  if (num_vertices != num_tex_coords_0 && (num_tex_coords_0 != 0)) {
    throw std::invalid_argument("#vertices != #tex_coords_0");
  }

  int offset = 0;
  const int vertices_size = num_vertices * 3 * sizeof(GLfloat);
  const int normals_size = num_normals * 3 * sizeof(GLfloat);
  const int diffuse_size = num_diffuse * 4 * sizeof(GLfloat);
  const int specular_size = num_specular * 4 * sizeof(GLfloat);
  const int shininess_size = num_shininess * 1 * sizeof(GLfloat);
  const int tex_coords_0_size = num_tex_coords_0 * 2 * sizeof(GLfloat);
  const int total_size =
      vertices_size + normals_size + diffuse_size + tex_coords_0_size;

  p_->vbo.allocate(total_size);

  if (num_vertices) {
    p_->vbo.write(offset, data.vertices.data(), vertices_size);
    p_->vertex_offset = offset;
    offset += vertices_size;
  }

  if (num_normals) {
    p_->vbo.write(offset, data.normals.data(), normals_size);
    p_->normal_offset = offset;
    offset += normals_size;
  }

  if (num_diffuse) {
    p_->vbo.write(offset, data.diffuse.data(), diffuse_size);
    p_->diffuse_offset = offset;
    offset += diffuse_size;
  }

  if (num_specular) {
    p_->vbo.write(offset, data.specular.data(), specular_size);
    p_->specular_offset = offset;
    offset += specular_size;
  }

  if (num_shininess) {
    p_->vbo.write(offset, data.shininess.data(), shininess_size);
    p_->shininess_offset = offset;
    offset += shininess_size;
  }

  if (num_tex_coords_0) {
    p_->vbo.write(offset, data.tex_coords_0.data(), tex_coords_0_size);
    p_->tex_coords_0_offset = offset;
    offset += tex_coords_0_size;
  }

  p_->num_vertices = num_vertices;
  p_->num_normals = num_normals;
  p_->num_diffuse = num_diffuse;
  p_->num_specular = num_specular;
  p_->num_shininess = num_shininess;
  p_->num_tex_coords_0 = num_tex_coords_0;

  p_->gl_mode = data.gl_mode;

  // load indices
  p_->num_indices = data.indices.size();
  if (p_->num_indices) {
    p_->index_buffer.create();
    p_->index_buffer.bind();

    if (num_vertices < 256) {
      // Optimize and convert the indices into a vector of unsigned shorts.
      std::vector<uint8_t> indices_byte(data.indices.begin(),
                                        data.indices.end());
      p_->index_buffer.allocate(indices_byte.data(),
                             p_->num_indices * sizeof(uint8_t));
      p_->index_type = GL_UNSIGNED_BYTE;
    } else if (num_vertices < 65536) {
      // Optimize and convert the indices into a vector of unsigned shorts.
      std::vector<uint16_t> indices_short(data.indices.begin(),
                                          data.indices.end());
      p_->index_buffer.allocate(indices_short.data(),
                             p_->num_indices * sizeof(uint16_t));
      p_->index_type = GL_UNSIGNED_SHORT;
    } else {
      p_->index_buffer.allocate(data.indices.data(),
                             p_->num_indices * sizeof(uint32_t));
      p_->index_type = GL_UNSIGNED_INT;
    }
  }

  // Initialize the bounding box
  p_->bounding_box = AxisAlignedBox();
  for (const auto& vertex : data.vertices) {
    p_->bounding_box.IncludePoint(QVector3D(vertex.x(), vertex.y(), vertex.z()));
  }

  for (Drawable* listener : p_->listeners) {
    listener->BoundingBoxChanged();
  }
}

QOpenGLBuffer* GeometryResource::VBO() { return &p_->vbo; }

QOpenGLBuffer* GeometryResource::IndexBuffer() {
  return p_->num_indices ? &p_->index_buffer : nullptr;
}

int GeometryResource::VertexOffset() const { return p_->vertex_offset; }

int GeometryResource::NumVertices() const { return p_->num_vertices; }

int GeometryResource::NormalOffset() const { return p_->normal_offset; }

int GeometryResource::NumNormals() const { return p_->num_normals; }

int GeometryResource::DiffuseOffset() const { return p_->diffuse_offset; }

int GeometryResource::NumDiffuse() const { return p_->num_diffuse; }

int GeometryResource::NumSpecular() const { return p_->num_specular; }

int GeometryResource::SpecularOffset() const { return p_->specular_offset; }

int GeometryResource::NumShininess() const { return p_->num_shininess; }

int GeometryResource::ShininessOffset() const { return p_->shininess_offset; }

int GeometryResource::TexCoords0Offset() const { return p_->tex_coords_0_offset; }

int GeometryResource::NumTexCoords0() const { return p_->num_tex_coords_0; }

int GeometryResource::NumIndices() const { return p_->num_indices; }

/**
 * Returns the type parameter to pass to glDrawElements()
 * Either GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
 */
GLenum GeometryResource::IndexType() const { return p_->index_type; }

/**
 * What kind of primitives are in this geometry (GL_POINTS, GL_LINE_STRIP,
 * ...)
 */
GLenum GeometryResource::GLMode() const { return p_->gl_mode; }

const AxisAlignedBox& GeometryResource::BoundingBox() const { return p_->bounding_box; }

void GeometryResource::AddListener(Drawable* listener) {
  p_->listeners.push_back(listener);
}

void GeometryResource::RemoveListener(Drawable* listener) {
  auto iter = std::find(p_->listeners.begin(), p_->listeners.end(), listener);
  if (iter != p_->listeners.end()) {
    p_->listeners.erase(iter);
  }
}

}  // namespace sv
