#ifndef SCENEVIEW_SHADER_UNIFORM_HPP__
#define SCENEVIEW_SHADER_UNIFORM_HPP__

#include <vector>
#include <map>

#include <QOpenGLShaderProgram>

namespace sceneview {

/**
 * Stores the location, type, and value for a GLSL shader uniform variable.
 *
 * The internals of this class are a bit messy, as multiple types are allowed.
 */
class ShaderUniform {
  public:
    enum class Type {
      kInvalid,
      kFloat,
      kInt
    };

    ShaderUniform();

    ShaderUniform(const QString& name);

    ~ShaderUniform();

    ShaderUniform(const ShaderUniform& other) = delete;

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

}  // namespace sceneview

#endif  // SCENEVIEW_SHADER_UNIFORM_HPP__
