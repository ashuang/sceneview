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

GeometryResource::GeometryResource(const QString& name) :
  name_(name),
  created_vbo_(false),
  vbo_(),
  index_buffer_(QOpenGLBuffer::IndexBuffer),
  vertex_offset_(0),
  normal_offset_(0),
  diffuse_offset_(0),
  specular_offset_(0),
  shininess_offset_(0),
  tex_coords_0_offset_(0),
  num_vertices_(0),
  num_normals_(0),
  num_diffuse_(0),
  num_specular_(0),
  num_shininess_(0),
  num_tex_coords_0_(0),
  num_indices_(0),
  gl_mode_(0),
  index_type_(GL_UNSIGNED_INT),
  bounding_box_() {
}

GeometryResource::~GeometryResource() {
  dbg("destroying geometry resource %s\n", name_.c_str());
  if (created_vbo_) {
    vbo_.destroy();
  }
  if (num_indices_) {
    index_buffer_.destroy();
  }
}

void GeometryResource::Load(const GeometryData& data) {
  if (!created_vbo_) {
    vbo_.create();
    created_vbo_ = true;
  }
  vbo_.bind();

  vertex_offset_ = 0;
  normal_offset_ = 0;
  diffuse_offset_ = 0;
  specular_offset_ = 0;
  shininess_offset_ = 0;
  tex_coords_0_offset_ = 0;

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

  vbo_.allocate(total_size);

  if (num_vertices) {
    vbo_.write(offset, data.vertices.data(), vertices_size);
    vertex_offset_ = offset;
    offset += vertices_size;
  }

  if (num_normals) {
    vbo_.write(offset, data.normals.data(), normals_size);
    normal_offset_ = offset;
    offset += normals_size;
  }

  if (num_diffuse) {
    vbo_.write(offset, data.diffuse.data(), diffuse_size);
    diffuse_offset_ = offset;
    offset += diffuse_size;
  }

  if (num_specular) {
    vbo_.write(offset, data.specular.data(), specular_size);
    specular_offset_ = offset;
    offset += specular_size;
  }

  if (num_shininess) {
    vbo_.write(offset, data.shininess.data(), shininess_size);
    shininess_offset_ = offset;
    offset += shininess_size;
  }

  if (num_tex_coords_0) {
    vbo_.write(offset, data.tex_coords_0.data(), tex_coords_0_size);
    tex_coords_0_offset_ = offset;
    offset += tex_coords_0_size;
  }

  num_vertices_ = num_vertices;
  num_normals_ = num_normals;
  num_diffuse_ = num_diffuse;
  num_specular_ = num_specular;
  num_shininess_ = num_shininess;
  num_tex_coords_0_ = num_tex_coords_0;

  gl_mode_ = data.gl_mode;

  // load indices
  num_indices_ = data.indices.size();
  if (num_indices_) {
    index_buffer_.create();
    index_buffer_.bind();

    if (num_vertices < 256) {
      // Optimize and convert the indices into a vector of unsigned shorts.
      std::vector<uint8_t> indices_byte(data.indices.begin(),
          data.indices.end());
      index_buffer_.allocate(indices_byte.data(),
          num_indices_ * sizeof(uint8_t));
      index_type_ = GL_UNSIGNED_BYTE;
    } else if (num_vertices < 65536) {
      // Optimize and convert the indices into a vector of unsigned shorts.
      std::vector<uint16_t> indices_short(data.indices.begin(),
          data.indices.end());
      index_buffer_.allocate(indices_short.data(),
          num_indices_ * sizeof(uint16_t));
      index_type_ = GL_UNSIGNED_SHORT;
    } else {
      index_buffer_.allocate(data.indices.data(),
          num_indices_ * sizeof(uint32_t));
      index_type_ = GL_UNSIGNED_INT;
    }
  }

  // Initialize the bounding box
  bounding_box_ = AxisAlignedBox();
  for (const auto& vertex : data.vertices) {
    bounding_box_.IncludePoint(QVector3D(vertex.x(), vertex.y(), vertex.z()));
  }

  for (Drawable* listener : listeners_) {
    listener->BoundingBoxChanged();
  }
}

QOpenGLBuffer* GeometryResource::IndexBuffer() {
  return num_indices_ ? &index_buffer_ : nullptr;
}

void GeometryResource::AddListener(Drawable* listener) {
  listeners_.push_back(listener);
}

void GeometryResource::RemoveListener(Drawable* listener) {
  auto iter = std::find(listeners_.begin(), listeners_.end(), listener);
  if (iter != listeners_.end()) {
    listeners_.erase(iter);
  }
}

}  // namespace sv
