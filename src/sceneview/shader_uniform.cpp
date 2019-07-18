// Copyright [2015] Albert Huang

#include "sceneview/internal_gl.hpp"
#include "sceneview/shader_uniform.hpp"

#include <vector>

namespace sv {

struct ShaderUniform::Priv {
  QString name;
  Type type;
  int location;
  Value value;
};

ShaderUniform::ShaderUniform() : p_(new Priv) {
  p_->type = Type::kInvalid;
  p_->location = -1;
}

ShaderUniform::ShaderUniform(const QString& name) : p_(new Priv) {
  p_->name = name;
  p_->type = Type::kInvalid;
  p_->location = -1;
}

ShaderUniform::~ShaderUniform() {
  Clear();
  delete p_;
}

void ShaderUniform::SetLocation(int location) { p_->location = location; }

int ShaderUniform::Location() const { return p_->location; }

ShaderUniform::Type ShaderUniform::ParamType() const { return p_->type; }

void ShaderUniform::Set(int val) {
  Clear();
  p_->type = Type::kInt;
  new (&(p_->value.int_data)) IntVec({val});
}

void ShaderUniform::Set(const std::vector<int>& val) {
  Clear();
  p_->type = Type::kInt;
  new (&(p_->value.int_data)) IntVec(val);
}

void ShaderUniform::Set(float val) {
  Clear();
  p_->type = Type::kFloat;
  new (&(p_->value.float_data)) FloatVec({val});
}

void ShaderUniform::Set(const std::vector<float>& val) {
  Clear();
  p_->type = Type::kFloat;
  new (&(p_->value.float_data)) FloatVec(val);
}

void ShaderUniform::Set(const QMatrix4x4& val) {
  Clear();
  p_->type = Type::kMat4f;
  new (&(p_->value.mat4f)) QMatrix4x4(val);
}

void ShaderUniform::Clear() {
  switch (p_->type) {
    case Type::kFloat:
      p_->value.float_data.~FloatVec();
      break;
    case Type::kInt:
      p_->value.int_data.~IntVec();
      break;
    case Type::kMat4f:
      p_->value.mat4f.~QMatrix4x4();
      break;
    case Type::kInvalid:
    default:
      break;
  }
}

void ShaderUniform::LoadToProgram(QOpenGLShaderProgram* program) {
  if (p_->location < 0) {
    p_->location = program->uniformLocation(p_->name);
    if (p_->location < 0) {
      printf("Warning: Unable to find uniform %s\n",
             p_->name.toStdString().c_str());
      return;
    }
  }

  switch (p_->type) {
    case Type::kFloat: {
      const FloatVec& vec = p_->value.float_data;
      switch (vec.size()) {
        case 1:
          glUniform1f(p_->location, vec[0]);
          break;
        case 2:
          glUniform2fv(p_->location, 1, vec.data());
          break;
        case 3:
          glUniform3fv(p_->location, 1, vec.data());
          break;
        case 4:
          glUniform4fv(p_->location, 1, vec.data());
          break;
        default:
          break;
      }
    } break;
    case Type::kInt: {
      const IntVec& vec = p_->value.int_data;
      switch (vec.size()) {
        case 1:
          glUniform1i(p_->location, vec[0]);
          break;
        case 2:
          glUniform2iv(p_->location, 1, vec.data());
          break;
        case 3:
          glUniform3iv(p_->location, 1, vec.data());
          break;
        case 4:
          glUniform4iv(p_->location, 1, vec.data());
          break;
        default:
          break;
      }
    } break;
    case Type::kMat4f:
      glUniformMatrix4fv(p_->location, 1, GL_FALSE,
                         p_->value.mat4f.constData());
      break;
    case Type::kInvalid:
    default:
      break;
  }

  GLenum gl_err = glGetError();
  if (gl_err != GL_NO_ERROR) {
    printf("Error loading shader uniform %s: %s\n",
           p_->name.toStdString().c_str(), sv::glErrorString(gl_err));
  }
}

ShaderUniform& ShaderUniform::operator=(const ShaderUniform& other) {
  Clear();
  p_->name = other.p_->name;
  p_->type = other.p_->type;
  p_->location = other.p_->location;
  switch (p_->type) {
    case Type::kInt:
      new (&(p_->value.int_data)) IntVec(other.p_->value.int_data);
      break;
    case Type::kFloat:
      new (&(p_->value.float_data)) FloatVec(other.p_->value.float_data);
      break;
    case Type::kMat4f:
      new (&(p_->value.mat4f)) QMatrix4x4(other.p_->value.mat4f);
      break;
    case Type::kInvalid:
    default:
      break;
  }
  return *this;
}

}  // namespace sv
