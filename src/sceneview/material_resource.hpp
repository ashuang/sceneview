// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_MATERIAL_RESOURCE_HPP__
#define SCENEVIEW_MATERIAL_RESOURCE_HPP__

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <sceneview/shader_resource.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/shader_uniform.hpp>

class QOpenGLTexture;

namespace sv {

struct StencilSettings;

/**
 * Controls the appearance of a Drawable.
 *
 * A material resource consists of:
 * - a reference to a shader.
 * - a set of parameters to pass to the shader.
 * - a set of standard parameters that affect OpenGL behavior outside of the
 *   shader.
 *
 * MaterialResource objects cannot be directly instantiated. Instead, use
 * ResourceManager or StockResources.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/material_resource.hpp
 */
class MaterialResource {
  public:
    typedef std::shared_ptr<MaterialResource> Ptr;

    typedef std::shared_ptr<QOpenGLTexture> TexturePtr;

    typedef std::map<QString, TexturePtr> TextureDictionary;

    typedef std::map<QString, TexturePtr> Textures;

    const ShaderResource::Ptr& Shader() { return shader_; }

    ShaderUniformMap& ShaderParameters() { return shader_parameters_; }

    void SetParam(const QString& name, int val);

    void SetParam(const QString& name, const std::vector<int>& val);

    void SetParam(const QString& name, float val);

    void SetParam(const QString& name,
        float val1, float val2);

    void SetParam(const QString& name,
        float val1, float val2, float val3);

    void SetParam(const QString& name,
        float val1, float val2, float val3, float val4);

    void SetParam(const QString& name, const std::vector<float>& val);

    void SetParam(const QString& name, const QMatrix4x4& value);

    void AddTexture(const QString& name, const TexturePtr& texture);

    const TextureDictionary& GetTextures() { return textures_; }

    /**
     * Sets whether or not to draw back-facing polygons.
     */
    void SetTwoSided(bool two_sided);

    bool TwoSided() const { return two_sided_; }

    void SetDepthWrite(bool val) { depth_write_ = val; }

    bool DepthWrite() const { return depth_write_; }

    void SetDepthTest(bool val) { depth_test_ = val; }

    bool DepthTest() const { return depth_test_; }

    void SetDepthFunc(GLenum func) { depth_func_ = func; }

    GLenum DepthFunc() const { return depth_func_; }

    /**
     * Enable stencil test and set its options.
     */
    void SetStencil(const StencilSettings& stencil);

    /**
     * Disable stencil test.
     */
    void DisableStencil();

    const StencilSettings* Stencil() const;

    void SetColorWrite(bool val) { color_write_ = val; }

    bool ColorWrite() const { return color_write_; }

    void SetPointSize(float size) { point_size_ = size; }

    float PointSize() const { return point_size_; }

    void SetLineWidth(float line_width) { line_width_ = line_width; }

    float LineWidth() const { return line_width_; }

    /**
     * Controls GL_BLEND
     *
     * @param value if true, then the GL_BLEND is enabled for this material. If
     * false, then GL_BLEND is disabled.
     */
    void SetBlend(bool value) { blend_ = value; }

    /**
     * Retrieve whether GL_BLEND should be enabled or disabled.
     */
    bool Blend() const { return blend_; }

    void SetBlendFunc(GLenum sfactor, GLenum dfactor) {
      blend_sfactor_ = sfactor;
      blend_dfactor_ = dfactor;
    }

    void BlendFunc(GLenum* sfactor, GLenum* dfactor) {
      *sfactor = blend_sfactor_;
      *dfactor = blend_dfactor_;
    }

  private:
    friend class ResourceManager;

    MaterialResource(const QString& name, ShaderResource::Ptr shader);

    const QString name_;

    ShaderResource::Ptr shader_;

    ShaderUniformMap shader_parameters_;

    bool two_sided_ = false;

    bool depth_write_ = true;

    bool depth_test_ = true;

    GLenum depth_func_ = GL_LESS;

    std::unique_ptr<StencilSettings> stencil_;

    bool color_write_ = true;

    float point_size_ = 1;

    float line_width_ = 1;

    bool blend_ = false;

    GLenum blend_sfactor_ = GL_ONE;

    GLenum blend_dfactor_ = GL_ZERO;

    TextureDictionary textures_;
};

/**
 * Face-specific stencil test settings
 */
struct StencilFaceSettings {
  // see: glStencilFunc() for func, func_ref, func_mask
  GLenum func = GL_ALWAYS;
  int func_ref = 0;
  unsigned int func_mask = 0xffffffff;

  // see: glStencilOp() for sfail, dpfail, dppass
  GLenum sfail = GL_KEEP;
  GLenum dpfail = GL_KEEP;
  GLenum dppass = GL_KEEP;

  // see: glStencilMask() for mask
  unsigned int mask = 0xffffffff;

  bool operator!=(const StencilFaceSettings& other) const;
  bool operator==(const StencilFaceSettings& other) const;
};

/**
 * https://www.khronos.org/opengl/wiki/Stencil_Test
 */
struct StencilSettings {
  StencilFaceSettings front;
  StencilFaceSettings back;

  bool operator!=(const StencilSettings& other) const;
  bool operator==(const StencilSettings& other) const;
};

}  // namespace sv

#endif  // SCENEVIEW_MATERIAL_RESOURCE_HPP__
