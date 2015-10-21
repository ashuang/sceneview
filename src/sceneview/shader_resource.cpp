// Copyright [2015] Albert Huang

#include "sceneview/shader_resource.hpp"

#include <string>
#include <vector>

#include <QFile>

namespace sv {

int kShaderMaxLights = 4;

ShaderResource::ShaderResource(const QString& name) :
  name_(name),
  program_() {
}

void ShaderResource::LoadFromFiles(const QString& prefix) {
  program_.reset(new QOpenGLShaderProgram());

  QString vshader_fname = prefix + ".vshader";
  QString fshader_fname = prefix + ".fshader";
  const std::string cprefix = prefix.toStdString();

  if (QFile(vshader_fname).exists()) {
    if (!program_->addShaderFromSourceFile(
          QOpenGLShader::Vertex, vshader_fname)) {
      throw std::runtime_error("Failed to load vertex shader " + cprefix +
          "\n" + program_->log().toStdString());
    }
  }

  if (QFile(fshader_fname).exists()) {
    if (!program_->addShaderFromSourceFile(
          QOpenGLShader::Fragment, fshader_fname)) {
      throw std::runtime_error("Failed to load vertex shader " + cprefix +
          "\n" + program_->log().toStdString());
    }
  }

  // TODO(albert) add other shader types
  if (!program_->link()) {
    throw std::runtime_error("Failed to link shader " + cprefix + "\n" +
        program_->log().toStdString());
  }
  if (!program_->bind()) {
    throw std::runtime_error("Failed to bind shader " + cprefix + "\n");
  }

  LoadLocations();
}

const ShaderStandardVariables& ShaderResource::StandardVariables() const {
  return locations_;
}

void ShaderResource::LoadLocations() {
  locations_.sv_proj_mat = program_->uniformLocation("sv_proj_mat");
  locations_.sv_view_mat = program_->uniformLocation("sv_view_mat");
  locations_.sv_view_mat_inv = program_->uniformLocation("sv_view_mat_inv");
  locations_.sv_model_mat = program_->uniformLocation("sv_model_mat");
  locations_.sv_mvp_mat = program_->uniformLocation("sv_mvp_mat");
  locations_.sv_mv_mat = program_->uniformLocation("sv_mv_mat");
  locations_.sv_model_normal_mat =
    program_->uniformLocation("sv_model_normal_mat");

  locations_.sv_lights.resize(kShaderMaxLights);

  for (int light_ind = 0; light_ind < kShaderMaxLights; ++light_ind) {
    const QString prefix = "sv_lights[" + QString::number(light_ind) + "].";
    ShaderLightLocation& light = locations_.sv_lights[light_ind];
    light.is_directional = program_->uniformLocation(prefix + "is_directional");
    light.position = program_->uniformLocation(prefix + "position");
    light.direction = program_->uniformLocation(prefix + "direction");
    light.color = program_->uniformLocation(prefix + "color");
    light.ambient = program_->uniformLocation(prefix + "ambient");
    light.attenuation = program_->uniformLocation(prefix + "attenuation");
    light.cone_angle = program_->uniformLocation(prefix + "cone_angle");
  }

  locations_.sv_vert_pos = program_->attributeLocation("sv_vert_pos");
  locations_.sv_normal = program_->attributeLocation("sv_normal");
  locations_.sv_diffuse = program_->attributeLocation("sv_diffuse");
  locations_.sv_ambient = program_->attributeLocation("sv_ambient");
  locations_.sv_specular = program_->attributeLocation("sv_specular");
  locations_.sv_shininess = program_->attributeLocation("sv_shininess");
  locations_.sv_tex_coords_0 = program_->attributeLocation("sv_tex_coords_0");
}

}  // namespace sv
