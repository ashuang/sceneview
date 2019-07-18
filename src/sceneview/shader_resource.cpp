// Copyright [2015] Albert Huang

#include "sceneview/shader_resource.hpp"

#include <string>
#include <vector>

#include <QFile>
#include <QTextStream>

namespace sv {

int kShaderMaxLights = 4;

struct ShaderResource::Priv {
  QString name;

  std::unique_ptr<QOpenGLShaderProgram> program;

  ShaderStandardVariables locations;
};

ShaderResource::ShaderResource(const QString& name) : p_(new Priv) {
  p_->name = name;
}

ShaderResource::~ShaderResource() { delete p_; }

const QString ShaderResource::Name() const { return p_->name; }

void ShaderResource::LoadFromFiles(const QString& prefix) {
  LoadFromFiles(prefix, "");
}

void ShaderResource::LoadFromFiles(const QString& prefix,
                                   const QString& preamble) {
  p_->program.reset(new QOpenGLShaderProgram());

  QFile vshader_file(prefix + ".vshader");
  QFile fshader_file(prefix + ".fshader");
  const std::string cprefix = prefix.toStdString();

  if (vshader_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString vshader_src = preamble + QTextStream(&vshader_file).readAll();

    if (!p_->program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                              vshader_src)) {
      throw std::runtime_error("Failed to load vertex shader " + cprefix +
                               "\n" + p_->program->log().toStdString());
    }
  }

  if (fshader_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString fshader_src = preamble + QTextStream(&fshader_file).readAll();
    if (!p_->program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                              fshader_src)) {
      throw std::runtime_error("Failed to load vertex shader " + cprefix +
                               "\n" + p_->program->log().toStdString());
    }
  }

  // TODO(albert) add other shader types
  if (!p_->program->link()) {
    throw std::runtime_error("Failed to link shader " + cprefix + "\n" +
                             p_->program->log().toStdString());
  }
  if (!p_->program->bind()) {
    throw std::runtime_error("Failed to bind shader " + cprefix + "\n");
  }

  LoadLocations();
}

QOpenGLShaderProgram* ShaderResource::Program() { return p_->program.get(); }

const ShaderStandardVariables& ShaderResource::StandardVariables() const {
  return p_->locations;
}

void ShaderResource::LoadLocations() {
  p_->locations.sv_proj_mat = p_->program->uniformLocation("sv_proj_mat");
  p_->locations.sv_view_mat = p_->program->uniformLocation("sv_view_mat");
  p_->locations.sv_view_mat_inv =
      p_->program->uniformLocation("sv_view_mat_inv");
  p_->locations.sv_model_mat = p_->program->uniformLocation("sv_model_mat");
  p_->locations.sv_mvp_mat = p_->program->uniformLocation("sv_mvp_mat");
  p_->locations.sv_mv_mat = p_->program->uniformLocation("sv_mv_mat");
  p_->locations.sv_model_normal_mat =
      p_->program->uniformLocation("sv_model_normal_mat");

  p_->locations.sv_lights.resize(kShaderMaxLights);

  for (int light_ind = 0; light_ind < kShaderMaxLights; ++light_ind) {
    const QString prefix = "sv_lights[" + QString::number(light_ind) + "].";
    ShaderLightLocation& light = p_->locations.sv_lights[light_ind];
    light.is_directional =
        p_->program->uniformLocation(prefix + "is_directional");
    light.position = p_->program->uniformLocation(prefix + "position");
    light.direction = p_->program->uniformLocation(prefix + "direction");
    light.color = p_->program->uniformLocation(prefix + "color");
    light.ambient = p_->program->uniformLocation(prefix + "ambient");
    light.specular = p_->program->uniformLocation(prefix + "specular");
    light.attenuation = p_->program->uniformLocation(prefix + "attenuation");
    light.cone_angle = p_->program->uniformLocation(prefix + "cone_angle");
  }

  p_->locations.sv_vert_pos = p_->program->attributeLocation("sv_vert_pos");
  p_->locations.sv_normal = p_->program->attributeLocation("sv_normal");
  p_->locations.sv_diffuse = p_->program->attributeLocation("sv_diffuse");
  p_->locations.sv_ambient = p_->program->attributeLocation("sv_ambient");
  p_->locations.sv_specular = p_->program->attributeLocation("sv_specular");
  p_->locations.sv_shininess = p_->program->attributeLocation("sv_shininess");
  p_->locations.sv_tex_coords_0 =
      p_->program->attributeLocation("sv_tex_coords_0");
}

}  // namespace sv
