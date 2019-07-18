// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_MATERIAL_RESOURCE_HPP__
#define SCENEVIEW_MATERIAL_RESOURCE_HPP__

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <sceneview/geometry_resource.hpp>
#include <sceneview/shader_resource.hpp>
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

  virtual ~MaterialResource();

  const ShaderResource::Ptr& Shader();

  ShaderUniformMap& ShaderParameters();

  void SetParam(const QString& name, int val);

  void SetParam(const QString& name, const std::vector<int>& val);

  void SetParam(const QString& name, float val);

  void SetParam(const QString& name, float val1, float val2);

  void SetParam(const QString& name, float val1, float val2, float val3);

  void SetParam(const QString& name, float val1, float val2, float val3,
                float val4);

  void SetParam(const QString& name, const std::vector<float>& val);

  void SetParam(const QString& name, const QMatrix4x4& value);

  void AddTexture(const QString& name, const TexturePtr& texture);

  const TextureDictionary& GetTextures();

  /**
   * Sets whether or not to draw back-facing polygons.
   */
  void SetTwoSided(bool two_sided);

  bool TwoSided() const;

  void SetDepthWrite(bool val);

  bool DepthWrite() const;

  void SetDepthTest(bool val);

  bool DepthTest() const;

  void SetDepthFunc(GLenum func);

  GLenum DepthFunc() const;

  /**
   * Enable stencil test and set its options.
   */
  void SetStencil(const StencilSettings& stencil);

  /**
   * Disable stencil test.
   */
  void DisableStencil();

  const StencilSettings* Stencil() const;

  void SetColorWrite(bool val);

  bool ColorWrite() const;

  void SetPointSize(float size);

  float PointSize() const;

  void SetLineWidth(float line_width);

  float LineWidth() const;

  /**
   * Controls GL_BLEND
   *
   * @param value if true, then the GL_BLEND is enabled for this material. If
   * false, then GL_BLEND is disabled.
   */
  void SetBlend(bool value);

  /**
   * Retrieve whether GL_BLEND should be enabled or disabled.
   */
  bool Blend() const;

  void SetBlendFunc(GLenum sfactor, GLenum dfactor);

  void BlendFunc(GLenum* sfactor, GLenum* dfactor);

 private:
  friend class ResourceManager;

  MaterialResource(const QString& name, ShaderResource::Ptr shader);

  struct Priv;

  Priv* p_;
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
