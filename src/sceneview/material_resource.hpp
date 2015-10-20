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

namespace sceneview {

/**
 * Controls the appearance of a mesh or other renderable object.
 *
 * A material resource consists of:
 * - a reference to a shader.
 * - a set of parameters to pass to the shader.
 * - a set of standard parameters that affect OpenGL behavior outside of the
 *   shader.
 */
class MaterialResource {
  public:
    typedef std::shared_ptr<MaterialResource> Ptr;

    typedef std::map<QString, QOpenGLTexture*> Textures;

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

    void SetTexture(const QString& name, QOpenGLTexture* texture);

    Textures* GetTextures() { return &textures_; }

    /**
     * Sets whether or not to draw back-facing polygons.
     */
    void SetTwoSided(bool two_sided);

    bool TwoSided() const { return two_sided_; }

    void SetDepthWrite(bool val) { depth_write_ = val; }

    bool DepthWrite() const { return depth_write_; }

    void SetDepthTest(bool val) { depth_test_ = val; }

    bool DepthTest() const { return depth_test_; }

    void SetColorWrite(bool val) { color_write_ = val; }

    bool ColorWrite() const { return color_write_; }

    void SetPointSize(float size) { point_size_ = size; }

    float PointSize() const { return point_size_; }

    void SetLineWidth(float line_width) { line_width_ = line_width; }

    float LineWidth() const { return line_width_; }

  private:
    friend class ResourceManager;

    MaterialResource(const QString& name, ShaderResource::Ptr shader);

    const QString name_;

    ShaderResource::Ptr shader_;

    ShaderUniformMap shader_parameters_;

    bool two_sided_;

    bool depth_write_;

    bool depth_test_;

    bool color_write_;

    float point_size_;

    float line_width_;

    std::map<QString, QOpenGLTexture*> textures_;
};

typedef std::pair<GeometryResource::Ptr, MaterialResource::Ptr>
    GeometryMaterialPair;

}  // namespace sceneview

#endif  // SCENEVIEW_MATERIAL_RESOURCE_HPP__
