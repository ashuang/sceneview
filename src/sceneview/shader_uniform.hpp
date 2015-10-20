// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SHADER_UNIFORM_HPP__
#define SCENEVIEW_SHADER_UNIFORM_HPP__

#include <vector>
#include <map>

#include <QOpenGLShaderProgram>

namespace sv {

/**
 * Stores the location, type, and value for a GLSL shader uniform variable.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/shader_uniform.hpp
 */
class ShaderUniform {
  public:
    enum class Type {
      kInvalid,
      kFloat,
      kInt
    };

    ShaderUniform();

    explicit ShaderUniform(const QString& name);

    ShaderUniform(const ShaderUniform& other) = delete;

    ~ShaderUniform();

    void SetLocation(int location) { location_ = location; }

    int Location() const { return location_; }

    Type ParamType() const { return type_; }

    void Set(int val);

    void Set(const std::vector<int>& val);

    void Set(float val);

    void Set(const std::vector<float>& val);

    void LoadToProgram(QOpenGLShaderProgram* program);

    ShaderUniform& operator=(const ShaderUniform& other);

  private:
    typedef std::vector<int> IntVec;
    typedef std::vector<float> FloatVec;

    union Value {
      IntVec int_data;
      FloatVec float_data;

      Value() {}
      ~Value() {}
    };

    void Clear();

    void CheckType(Type expected);

    QString name_;
    Type type_;
    int location_;
    Value value_;
};

typedef std::map<QString, ShaderUniform> ShaderUniformMap;

}  // namespace sv

#endif  // SCENEVIEW_SHADER_UNIFORM_HPP__
