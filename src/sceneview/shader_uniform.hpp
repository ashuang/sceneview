// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SHADER_UNIFORM_HPP__
#define SCENEVIEW_SHADER_UNIFORM_HPP__

#include <map>
#include <vector>

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
  enum class Type { kInvalid, kFloat, kInt, kMat4f };

  ShaderUniform();

  explicit ShaderUniform(const QString& name);

  ShaderUniform(const ShaderUniform& other) = delete;

  ~ShaderUniform();

  void SetLocation(int location);

  int Location() const;

  Type ParamType() const;

  void Set(int val);

  void Set(const std::vector<int>& val);

  void Set(float val);

  void Set(const std::vector<float>& val);

  void Set(const QMatrix4x4& val);

  void LoadToProgram(QOpenGLShaderProgram* program);

  ShaderUniform& operator=(const ShaderUniform& other);

 private:
  typedef std::vector<int> IntVec;
  typedef std::vector<float> FloatVec;

  union Value {
    IntVec int_data;
    FloatVec float_data;
    QMatrix4x4 mat4f;

    Value() {}
    ~Value() {}
  };

  void Clear();

  void CheckType(Type expected);

  struct Priv;
  Priv* p_;
};

typedef std::map<QString, ShaderUniform> ShaderUniformMap;

}  // namespace sv

#endif  // SCENEVIEW_SHADER_UNIFORM_HPP__
