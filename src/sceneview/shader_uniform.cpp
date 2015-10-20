// Copyright [2015] Albert Huang

#include "sceneview/internal_gl.h"
#include "sceneview/shader_uniform.hpp"

#include <vector>

namespace sceneview {

ShaderUniform::ShaderUniform() :
  name_(),
  type_(Type::kInvalid),
  location_(-1) {}

ShaderUniform::ShaderUniform(const QString& name) :
  name_(name),
  type_(Type::kInvalid),
  location_(-1) {
}

ShaderUniform::~ShaderUniform() {
  Clear();
}

void ShaderUniform::Set(int val) {
  Clear();
  type_ = Type::kInt;
  new(&(value_.int_data)) IntVec({val});
}

void ShaderUniform::Set(const std::vector<int>& val) {
  Clear();
  type_ = Type::kInt;
  new(&(value_.int_data)) IntVec(val);
}

void ShaderUniform::Set(float val) {
  Clear();
  type_ = Type::kFloat;
  new(&(value_.float_data)) FloatVec({val});
}

void ShaderUniform::Set(const std::vector<float>& val) {
  Clear();
  type_ = Type::kFloat;
  new(&(value_.float_data)) FloatVec(val);
}

void ShaderUniform::Clear() {
  switch (type_) {
    case Type::kFloat:
      value_.float_data.~FloatVec();
      break;
    case Type::kInt:
      value_.int_data.~IntVec();
      break;
    case Type::kInvalid:
    default:
      break;
  }
}

void ShaderUniform::LoadToProgram(QOpenGLShaderProgram* program) {
  if (location_ < 0) {
    location_ = program->uniformLocation(name_);
    if (location_ < 0) {
      printf("Warning: Unable to find uniform %s\n",
          name_.toStdString().c_str());
      return;
    }
  }

  switch (type_) {
    case Type::kFloat:
      {
        const FloatVec& vec = value_.float_data;
        switch (vec.size()) {
          case 1:
            glUniform1f(location_, vec[0]);
            break;
          case 2:
            glUniform2fv(location_, 1, vec.data());
            break;
          case 3:
            glUniform3fv(location_, 1, vec.data());
            break;
          case 4:
            glUniform4fv(location_, 1, vec.data());
            break;
          default:
            break;
        }
      }
      break;
    case Type::kInt:
      {
        const IntVec& vec = value_.int_data;
        switch (vec.size()) {
          case 1:
            glUniform1i(location_, vec[0]);
            break;
          case 2:
            glUniform2iv(location_, 1, vec.data());
            break;
          case 3:
            glUniform3iv(location_, 1, vec.data());
            break;
          case 4:
            glUniform4iv(location_, 1, vec.data());
            break;
          default:
            break;
        }
      }
      break;
    case Type::kInvalid:
    default:
      break;
  }

  GLenum gl_err = glGetError();
  if (gl_err != GL_NO_ERROR) {
    printf("Error loading shader uniform %s: %s\n",
        name_.toStdString().c_str(),
        sceneview::glErrorString(gl_err));
  }
}

ShaderUniform& ShaderUniform::operator=(const ShaderUniform& other) {
  Clear();
  name_ = other.name_;
  type_ = other.type_;
  location_ = other.location_;
  switch (type_) {
    case Type::kInt:
      new(&(value_.int_data)) IntVec(other.value_.int_data);
      break;
    case Type::kFloat:
      new(&(value_.float_data)) FloatVec(other.value_.float_data);
      break;
    case Type::kInvalid:
    default:
      break;
  }
  return *this;
}

}  // namespace sceneview
