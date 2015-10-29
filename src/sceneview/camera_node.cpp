// Copyright [2015] Albert Huang

#include "sceneview/camera_node.hpp"

#include <cmath>
#include <iostream>

#include <QVector4D>

namespace sv {

CameraNode::CameraNode(const QString& name) :
  SceneNode(name),
  look_(),
  up_(),
  look_at_(),
  viewport_width_(0),
  viewport_height_(0),
  proj_type_(ProjectionType::kPerspective),
  vfov_deg_(50),
  z_near_(0.1),
  z_far_(10000) {
    LookAt(QVector3D(0, 0, 0),
        QVector3D(1, 0, 0),
        QVector3D(0, 0, 1));
}

void CameraNode::CopyFrom(const CameraNode& other) {
  viewport_width_ = other.viewport_width_;
  viewport_height_ = other.viewport_height_;
  vfov_deg_ = other.vfov_deg_;
  z_near_ = other.z_near_;
  z_far_ = other.z_far_;
  projection_matrix_ = other.projection_matrix_;

  look_ = other.look_;
  up_ = other.up_;
  SceneNode::SetTranslation(other.Translation());
  SceneNode::SetRotation(other.Rotation());
}

void CameraNode::SetViewportSize(int width, int height) {
  viewport_width_ = width;
  viewport_height_ = height;

  ComputeProjectionMatrix();
}

QSize CameraNode::GetViewportSize() const {
  return QSize(viewport_width_, viewport_height_);
}

void CameraNode::SetProjectionParams(ProjectionType type,
    double vfov_deg, double z_near, double z_far) {
  if (vfov_deg < 1e-6) {
    throw std::invalid_argument("invalid vfov");
  }
  vfov_deg_ = vfov_deg;
  z_near_ = z_near;
  z_far_ = z_far;
  proj_type_ = type;
  ComputeProjectionMatrix();
}

static QQuaternion QuatFromRot(const QMatrix3x3& rot) {
  const double trace = rot(0, 0) + rot(1, 1) + rot(2, 2);
  if (trace > 0) {
    const double qw = sqrt(1 + rot(0, 0) + rot(1, 1) + rot(2, 2)) / 2;
    const double qx = (rot(2, 1) - rot(1, 2)) / (qw * 4);
    const double qy = (rot(0, 2) - rot(2, 0)) / (qw * 4);
    const double qz = (rot(1, 0) - rot(0, 1)) / (qw * 4);
    return QQuaternion(qw, qx, qy, qz);
  }
  if ((rot(0, 0) > rot(1, 1)) && (rot(0, 0) > rot(2, 2))) {
    const float qx4 = sqrt(1.0 + rot(0, 0) - rot(1, 1) - rot(2, 2)) * 2;
    const double qw = (rot(2, 1) - rot(1, 2)) / qx4;
    const double qx = 0.25 * qx4;
    const double qy = (rot(0, 1) + rot(1, 0)) / qx4;
    const double qz = (rot(0, 2) + rot(2, 0)) / qx4;
    return QQuaternion(qw, qx, qy, qz);
  } else if (rot(1, 1) > rot(2, 2)) {
    const float qy4 = sqrt(1.0 + rot(1, 1) - rot(0, 0) - rot(2, 2)) * 2;
    const double qw = (rot(0, 2) - rot(2, 0)) / qy4;
    const double qx = (rot(0, 1) + rot(1, 0)) / qy4;
    const double qy = 0.25 * qy4;
    const double qz = (rot(1, 2) + rot(2, 1)) / qy4;
    return QQuaternion(qw, qx, qy, qz);
  } else {
    const float qz4 = sqrt(1.0 + rot(2, 2) - rot(0, 0) - rot(1, 1)) * 2;
    const double qw = (rot(1, 0) - rot(0, 1)) / qz4;
    const double qx = (rot(0, 2) + rot(2, 0)) / qz4;
    const double qy = (rot(1, 2) + rot(2, 1)) / qz4;
    const double qz = 0.25 * qz4;
    return QQuaternion(qw, qx, qy, qz);
  }
}

static QMatrix3x3 RotFromQuat(const QQuaternion& quat) {
  const float norm = quat.length();
  if (fabs(norm) < 1e-10) {
    QMatrix3x3 result;
    result.fill(0.0f);
    return result;
  }

  const float scale = 1 / norm;
  const float x = quat.x() * scale;
  const float y = quat.y() * scale;
  const float z = quat.z() * scale;
  const float w = quat.scalar() * scale;

  const float x2 = x * x;
  const float y2 = y * y;
  const float z2 = z * z;
  const float w2 = w * w;
  const float xy = 2 * x * y;
  const float xz = 2 * x * z;
  const float yz = 2 * y * z;
  const float wx = 2 * w * x;
  const float wy = 2 * w * y;
  const float wz = 2 * w * z;

  const float data[9] = {
    w2+x2-y2-z2, xy-wz, xz+wy,
    xy+wz, w2-x2+y2-z2, yz-wx,
    xz-wy, yz+wx, w2-x2-y2+z2,
  };

  return QMatrix3x3(data);
}

void CameraNode::LookAt(const QVector3D& eye, const QVector3D& look_at,
    const QVector3D& up_denorm) {
  look_at_ = look_at;
  const QVector3D look_denorm = look_at - eye;
  if (look_denorm.length() < 1e-9) {
    throw std::invalid_argument("eye and look_at are too close!");
  }
  const QVector3D look = look_denorm.normalized();

  const QVector3D right = QVector3D::crossProduct(
      look, up_denorm).normalized();
  const QVector3D up = QVector3D::crossProduct(
      right, look).normalized();
  const float rot_data[] = { right.x(), up.x(), -look.x(),
                       right.y(), up.y(), -look.y(),
                       right.z(), up.z(), -look.z() };
  SetTranslation(eye);
  SetRotation(QuatFromRot(QMatrix3x3(rot_data)));

  if (proj_type_ == kOrthographic) {
    ComputeProjectionMatrix();
  }
}

QVector3D CameraNode::GetLookDir() const {
  return look_;
}

QVector3D CameraNode::GetLookAt() const {
  return look_at_;
}

QVector3D CameraNode::GetUpDir() const {
  return up_;
}

QVector3D CameraNode::Unproject(double x, double y) {
  // Window coordinates to screen coordinates
  const double screen_x = x;
  const double screen_y = viewport_height_ - y;

  // Screen coordinates to normalized device coordinates (NDC).
  const QVector4D clip_vec(2 * screen_x / viewport_width_ - 1,
     2 * screen_y / viewport_height_ - 1,
      1, 1);
  // NDC to world coordinates
  const QMatrix4x4 vp_inverse = GetViewProjectionMatrix().inverted();
  const QVector4D pworld = vp_inverse * clip_vec;
  // World coordinates to Cartesian world coordinates.
  const QVector3D world(pworld.x() / pworld.w(),
      pworld.y() / pworld.w(),
      pworld.z() / pworld.w());
  return (world - Translation()).normalized();
}

QMatrix4x4 CameraNode::GetProjectionMatrix() {
  return projection_matrix_;
}

QMatrix4x4 CameraNode::GetViewMatrix() {
  QMatrix4x4 result;
  result.translate(Translation());
  result.rotate(Rotation());
  return result.inverted();
}

QMatrix4x4 CameraNode::GetViewProjectionMatrix() {
  return projection_matrix_ * GetViewMatrix();
}

void CameraNode::ComputeProjectionMatrix() {
  projection_matrix_.fill(0.0f);
  if (viewport_height_ <= 0 || viewport_width_ <= 0 || vfov_deg_ <= 0) {
    return;
  }

  const double aspect = static_cast<double>(viewport_width_) / viewport_height_;
  const double vfov = vfov_deg_ * M_PI / 180;
  const double delta_z = z_far_ - z_near_;

  switch (proj_type_) {
    case ProjectionType::kOrthographic:
      {
        const double dist_to_look_at = (Translation() - look_at_).length();
        const double bottom = dist_to_look_at * tan(vfov / 2);
        const double right = bottom * aspect;
        projection_matrix_(0, 0) = 1 / right;
        projection_matrix_(1, 1) = 1 / bottom;
        projection_matrix_(2, 2) = -2 / delta_z;
        projection_matrix_(2, 3) = -(z_far_ + z_near_) / delta_z;
        projection_matrix_(3, 3) = 1;
      }
      break;
    case ProjectionType::kPerspective:
    default:
      {
        const double cotan = 1 / tan(vfov / 2);
        projection_matrix_(0, 0) = cotan / aspect;
        projection_matrix_(1, 1) = cotan;
        projection_matrix_(2, 2) = -(z_far_ + z_near_) / delta_z;
        projection_matrix_(3, 2) = -1;
        projection_matrix_(2, 3) = -2 * z_near_ * z_far_ / delta_z;
      }
      break;
  }
}

void CameraNode::SetTranslation(const QVector3D& vec) {
  SceneNode::SetTranslation(vec);
}

void CameraNode::SetRotation(const QQuaternion& quat) {
  SceneNode::SetRotation(quat);
  const QMatrix3x3 rot = RotFromQuat(quat);
  look_ = QVector3D(-rot(0, 2), -rot(1, 2), -rot(2, 2));
  up_ = QVector3D(rot(0, 1), rot(1, 1), rot(2, 1));
}

}  // namespace sv
