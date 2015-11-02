// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SHADER_RESOURCE_HPP__
#define SCENEVIEW_SHADER_RESOURCE_HPP__

#include <memory>
#include <vector>

#include <QOpenGLShaderProgram>

namespace sv {

extern int kShaderMaxLights;

/**
 * Holds the GLSL locations of light parameters in a shader program.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/shader_resource.hpp
 */
struct ShaderLightLocation {
  int is_directional;
  int position;
  int direction;
  int color;
  int ambient;
  int attenuation;
  int cone_angle;
};

/**
 * Holds the GLSL locations of shader variables.
 *
 * Queried via `glGetUniformLocation()` and `glGetVertexAttribPointer()`.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/shader_resource.hpp
 */
struct ShaderStandardVariables {
  // ============ Uniform variables
  // Automatically populated based on the scene graph structure

  /** Projection matrix.
   * Transforms from camera eye space to clip space.
   * Type: mat4
   */
  int sv_proj_mat;

  /** View matrix.
   * Transforms from world space to camera eye space.
   * Type: mat4
   */
  int sv_view_mat;

  /** View matrix inverse.
   * Transforms from camera eye space to world space.
   * Type: mat4
   */
  int sv_view_mat_inv;

  /** Model matrix.
   * Transforms from model space to world space.
   * Type: mat4
   **/
  int sv_model_mat;

  /**
   * Model-view-projection matrix.
   * Composition of matrices: mvp = projection * view * model
   * Type: mat4
   */
  int sv_mvp_mat;

  /**
   * Model-view matrix.
   * Composition of matrices: mv = view * model
   * Type: mat4
   */
  int sv_mv_mat;

  /**
   * Model normal matrix.
   * Transforms normals from model space to world space.
   * Type: mat3
   */
  int sv_model_normal_mat;

  // Lights
  std::vector<ShaderLightLocation> sv_lights;

  // ============== Per-vertex attributes
  // Automatically populated based on the object geometry

  /**
   * Vertex position.
   */
  int sv_vert_pos;

  /**
   * Vertex normal vector.
   */
  int sv_normal;

  /**
   * Per-vertex diffuse color.
   */
  int sv_diffuse;

  /**
   * Per-vertex ambient color.
   */
  int sv_ambient;

  /**
   * Per-vertex specular color.
   */
  int sv_specular;

  /**
   * Per-vertex shininess.
   */
  int sv_shininess;

  /**
   * Texture coordinates set 0
   */
  int sv_tex_coords_0;
};

/**
 * An OpenGL shader program.
 *
 * ShaderResource objects cannot be directly instantiated. Instead, use
 * ResourceManager or StockResources.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/shader_resource.hpp
 */
class ShaderResource {
  public:
    typedef std::shared_ptr<ShaderResource> Ptr;

    const QString Name() const { return name_; }

    /**
     * Loads a vertex shader and fragment shader into this resource.
     *
     * @prefix filename prefix. ".vshader" will automatically be added to the
     *         vertex shader filename, and ".fshader" will automatically be
     *         added to the fragment shader filename.
     */
    void LoadFromFiles(const QString& prefix);

    /**
     * Loads a vertex shader and fragment shader into this resource.
     *
     * @param prefix filename prefix. ".vshader" will automatically be added to
     *        the vertex shader filename, and ".fshader" will automatically be
     *        added to the fragment shader filename.
     * @param preamble text to prepend to both the vertex and fragment shaders
     *        before compiling. You can use this to define preprocessor
     *        constants, etc.
     */
    void LoadFromFiles(const QString& prefix, const QString& preamble);

    QOpenGLShaderProgram* Program() { return program_.get(); }

    const ShaderStandardVariables& StandardVariables() const;

  private:
    friend class ResourceManager;

    explicit ShaderResource(const QString& name);

    void LoadLocations();

    QString name_;

    std::unique_ptr<QOpenGLShaderProgram> program_;

    ShaderStandardVariables locations_;
};

}  // namespace sv

#endif  // SCENEVIEW_SHADER_RESOURCE_HPP__
