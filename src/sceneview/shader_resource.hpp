// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SHADER_RESOURCE_HPP__
#define SCENEVIEW_SHADER_RESOURCE_HPP__

#include <memory>
#include <vector>

#include <QOpenGLShaderProgram>

namespace sv {

extern int kShaderMaxLights;

struct ShaderLightLocation {
  int is_directional;
  int position;
  int direction;
  int color;
  int ambient;
  int attenuation;
  int cone_angle;
};

struct ShaderStandardVariables {
  // ============ Uniform variables
  // Automatically populated based on the scene graph structure

  /** Projection matrix.
   * Transforms from camera eye space to clip space.
   * Type: mat4
   */
  int b3_proj_mat;

  /** View matrix.
   * Transforms from world space to camera eye space.
   * Type: mat4
   */
  int b3_view_mat;

  /** View matrix inverse.
   * Transforms from camera eye space to world space.
   * Type: mat4
   */
  int b3_view_mat_inv;

  /** Model matrix.
   * Transforms from model space to world space.
   * Type: mat4
   **/
  int b3_model_mat;

  /**
   * Model-view-projection matrix.
   * Composition of matrices: mvp = projection * view * model
   * Type: mat4
   */
  int b3_mvp_mat;

  /**
   * Model-view matrix.
   * Composition of matrices: mv = view * model
   * Type: mat4
   */
  int b3_mv_mat;

  /**
   * Model normal matrix.
   * Transforms normals from model space to world space.
   * Type: mat3
   */
  int b3_model_normal_mat;

  // Lights
  std::vector<ShaderLightLocation> b3_lights;

  // ============== Per-vertex attributes
  // Automatically populated based on the mesh geometry

  /**
   * Vertex position.
   */
  int b3_vert_pos;

  /**
   * Vertex normal vector.
   */
  int b3_normal;

  /**
   * Per-vertex diffuse color.
   */
  int b3_diffuse;

  /**
   * Per-vertex ambient color.
   */
  int b3_ambient;

  /**
   * Per-vertex specular color.
   */
  int b3_specular;

  /**
   * Per-vertex shininess.
   */
  int b3_shininess;

  /**
   * Texture coordinates set 0
   */
  int b3_tex_coords_0;
};

class ShaderResource {
  public:
    typedef std::shared_ptr<ShaderResource> Ptr;

    const QString Name() const { return name_; }

    /**
     *
     */
    void LoadFromFiles(const QString& prefix);

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
