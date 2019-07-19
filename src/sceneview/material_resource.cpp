// Copyright [2015] Albert Huang

#include "sceneview/material_resource.hpp"

#include <vector>

namespace sv {

bool StencilFaceSettings::operator!=(const StencilFaceSettings& other) const {
  return func != other.func || func_ref != other.func_ref ||
         func_mask != other.func_mask || sfail != other.sfail ||
         dpfail != other.dpfail || dppass != other.dppass || mask != other.mask;
}

bool StencilFaceSettings::operator==(const StencilFaceSettings& other) const {
  return !(*this != other);
}

bool StencilSettings::operator!=(const StencilSettings& other) const {
  return front != other.front || back != other.back;
}

bool StencilSettings::operator==(const StencilSettings& other) const {
  return !(*this != other);
}

struct MaterialResource::Priv {
  QString name;

  ShaderResource::Ptr shader;

  ShaderUniformMap shader_parameters;

  bool two_sided = false;

  bool depth_write = true;

  bool depth_test = true;

  GLenum depth_func = GL_LESS;

  std::unique_ptr<StencilSettings> stencil;

  bool color_write = true;

  float point_size = 1;

  float line_width = 1;

  bool blend = false;

  GLenum blend_sfactor = GL_ONE;

  GLenum blend_dfactor = GL_ZERO;

  TextureDictionary textures;
};

MaterialResource::~MaterialResource() { delete p_; }

const ShaderResource::Ptr& MaterialResource::Shader() { return p_->shader; }

ShaderUniformMap& MaterialResource::ShaderParameters() {
  return p_->shader_parameters;
}

MaterialResource::MaterialResource(const QString& name,
                                   ShaderResource::Ptr shader)
    : p_(new Priv()) {
  p_->name = name;
  p_->shader = shader;
}

template <typename ValueType>
void SUMapSet(ShaderUniformMap* psu_map, const QString& name, ValueType val) {
  ShaderUniformMap& su_map = *psu_map;
  auto iter = su_map.find(name);
  if (iter != su_map.end()) {
    iter->second.Set(val);
  } else {
    su_map[name] = ShaderUniform(name);
    su_map[name].Set(val);
  }
}

void MaterialResource::SetStencil(const StencilSettings& stencil) {
  if (p_->stencil) {
    *p_->stencil = stencil;
  } else {
    p_->stencil.reset(new StencilSettings(stencil));
  }
}

void MaterialResource::DisableStencil() { p_->stencil.reset(); }

const StencilSettings* MaterialResource::Stencil() const {
  return p_->stencil ? p_->stencil.get() : nullptr;
}

void MaterialResource::SetColorWrite(bool val) { p_->color_write = val; }

bool MaterialResource::ColorWrite() const { return p_->color_write; }

void MaterialResource::SetPointSize(float size) { p_->point_size = size; }

float MaterialResource::PointSize() const { return p_->point_size; }

void MaterialResource::SetLineWidth(float line_width) {
  p_->line_width = line_width;
}

float MaterialResource::LineWidth() const { return p_->line_width; }

/**
 * Controls GL_BLEND
 *
 * @param value if true, then the GL_BLEND is enabled for this material. If
 * false, then GL_BLEND is disabled.
 */
void MaterialResource::SetBlend(bool value) { p_->blend = value; }

/**
 * Retrieve whether GL_BLEND should be enabled or disabled.
 */
bool MaterialResource::Blend() const { return p_->blend; }

void MaterialResource::SetBlendFunc(GLenum sfactor, GLenum dfactor) {
  p_->blend_sfactor = sfactor;
  p_->blend_dfactor = dfactor;
}

void MaterialResource::BlendFunc(GLenum* sfactor, GLenum* dfactor) {
  *sfactor = p_->blend_sfactor;
  *dfactor = p_->blend_dfactor;
}

void MaterialResource::SetParam(const QString& name, int val) {
  SUMapSet(&p_->shader_parameters, name, val);
}

void MaterialResource::SetParam(const QString& name,
                                const std::vector<int>& val) {
  SUMapSet(&p_->shader_parameters, name, val);
}

void MaterialResource::SetParam(const QString& name, float val) {
  SUMapSet(&p_->shader_parameters, name, val);
}

void MaterialResource::SetParam(const QString& name, float val1, float val2) {
  SUMapSet(&p_->shader_parameters, name, std::vector<float>({val1, val2}));
}

void MaterialResource::SetParam(const QString& name, float val1, float val2,
                                float val3) {
  SUMapSet(&p_->shader_parameters, name,
           std::vector<float>({val1, val2, val3}));
}

void MaterialResource::SetParam(const QString& name, float val1, float val2,
                                float val3, float val4) {
  SUMapSet(&p_->shader_parameters, name,
           std::vector<float>({val1, val2, val3, val4}));
}

void MaterialResource::SetParam(const QString& name,
                                const std::vector<float>& val) {
  SUMapSet(&p_->shader_parameters, name, val);
}

void MaterialResource::SetParam(const QString& name, const QMatrix4x4& value) {
  SUMapSet(&p_->shader_parameters, name, value);
}

void MaterialResource::AddTexture(const QString& name,
                                  const MaterialResource::TexturePtr& texture) {
  if (texture == nullptr) {
    auto iter = p_->textures.find(name);
    if (iter != p_->textures.end()) {
      p_->textures.erase(iter);
    }
  } else {
    p_->textures[name] = texture;
  }
}

const MaterialResource::TextureDictionary& MaterialResource::GetTextures() {
  return p_->textures;
}

void MaterialResource::SetTwoSided(bool two_sided) {
  p_->two_sided = two_sided;
}

bool MaterialResource::TwoSided() const { return p_->two_sided; }

void MaterialResource::SetDepthWrite(bool val) { p_->depth_write = val; }

bool MaterialResource::DepthWrite() const { return p_->depth_write; }

void MaterialResource::SetDepthTest(bool val) { p_->depth_test = val; }

bool MaterialResource::DepthTest() const { return p_->depth_test; }

void MaterialResource::SetDepthFunc(GLenum func) { p_->depth_func = func; }

GLenum MaterialResource::DepthFunc() const { return p_->depth_func; }

}  // namespace sv
