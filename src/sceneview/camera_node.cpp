// Copyright [2015] Albert Huang

#include "sceneview/camera_node.hpp"

#include <cmath>
#include <iostream>

#include <QVector4D>

namespace sv {

struct CameraNode::Priv {
  static const AxisAlignedBox kBoundingBox;

  QVector3D look;
  QVector3D up;
  QVector3D look_at;

  int viewport_width = 0;
  int viewport_height = 0;

  ProjectionType proj_type;

  double vfov_deg;

  double z_near;
  double z_far;

  QMatrix4x4 projection_matrix;
};

const AxisAlignedBox CameraNode::Priv::kBoundingBox;

CameraNode::CameraNode(const QString& name) :
  SceneNode(name),
  p_(new Priv) {
  p_->proj_type = ProjectionType::kPerspective;
  p_->vfov_deg = 50;
  p_->z_near = 0.1;
  p_->z_far = 10000;

  // Choose an initial camera position/orientation such that the
  // translation/rotation are both identity.
  LookAt(QVector3D(0, 0, 0), QVector3D(0, 0, -1), QVector3D(0, 1, 0));
}

CameraNode::~CameraNode() {
  delete p_;
}

void CameraNode::CopyFrom(const CameraNode& other) {
  p_->viewport_width = other.p_->viewport_width;
  p_->viewport_height = other.p_->viewport_height;
  p_->vfov_deg = other.p_->vfov_deg;
  p_->z_near = other.p_->z_near;
  p_->z_far = other.p_->z_far;
  p_->projection_matrix = other.p_->projection_matrix;

  p_->look = other.p_->look;
  p_->up = other.p_->up;
  SceneNode::SetTranslation(other.Translation());
  SceneNode::SetRotation(other.Rotation());
}

void CameraNode::SetViewportSize(int width, int height) {
  if (p_->viewport_width == width && p_->viewport_height == height) {
    return;
  }
  p_->viewport_width = width;
  p_->viewport_height = height;

  ComputeProjectionMatrix();
}

QSize CameraNode::GetViewportSize() const {
  return QSize(p_->viewport_width, p_->viewport_height);
}

void CameraNode::SetPerspective(double vfov_deg, double z_near, double z_far) {
  if (vfov_deg < 1e-6) {
    throw std::invalid_argument("invalid vfov");
  }
  p_->vfov_deg = vfov_deg;
  p_->z_near = z_near;
  p_->z_far = z_far;
  p_->proj_type = kPerspective;
  ComputeProjectionMatrix();
}

void CameraNode::SetOrthographic(double vfov_deg, double z_near, double z_far) {
  if (vfov_deg < 1e-6) {
    throw std::invalid_argument("invalid vfov");
  }
  p_->vfov_deg = vfov_deg;
  p_->z_near = z_near;
  p_->z_far = z_far;
  p_->proj_type = kOrthographic;
  ComputeProjectionMatrix();
}

void CameraNode::SetManual(const QMatrix4x4& proj_mat) {
  p_->projection_matrix = proj_mat;
  p_->proj_type = kManual;
}

CameraNode::ProjectionType CameraNode::GetProjectionType() const { return p_->proj_type; }

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
  p_->look_at = look_at;
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

  if (p_->proj_type == kOrthographic) {
    ComputeProjectionMatrix();
  }
}

double CameraNode::GetVFovDeg() const { return p_->vfov_deg; }

/**
 * Retrieve the near clipping plane.
 */
double CameraNode::GetZNear() const { return p_->z_near; }

/**
 * Retrieve the far clipping plane.
 */
double CameraNode::GetZFar() const { return p_->z_far; }

QVector3D CameraNode::GetLookDir() const {
  return p_->look;
}

QVector3D CameraNode::GetLookAt() const {
  return p_->look_at;
}

QVector3D CameraNode::GetUpDir() const {
  return p_->up;
}

QVector3D CameraNode::Unproject(double x, double y) {
  // Window coordinates to screen coordinates
  const double screen_x = x;
  const double screen_y = p_->viewport_height - y;

  // Screen coordinates to normalized device coordinates (NDC).
  const QVector4D clip_vec(2 * screen_x / p_->viewport_width - 1,
     2 * screen_y / p_->viewport_height - 1,
      1, 1);
  // NDC to world coordinates
  const QMatrix4x4 vp_inverse = GetViewProjectionMatrix().inverted();
  const QVector4D pworld = vp_inverse * clip_vec;
  // World coordinates to Cartesian world coordinates.
  const QVector3D world(pworld.x() / pworld.w(),
      pworld.y() / pworld.w(),
      pworld.z() / pworld.w());
  const QVector3D eye = WorldTransform().map(QVector3D(0, 0, 0));
  return (world - eye).normalized();
}

QVector3D CameraNode::Unproject(double x, double y, double z) {
  // Window coordinates to screen coordinates
  const double screen_x = x;
  const double screen_y = p_->viewport_height - y;

  // Screen coordinates to normalized device coordinates (NDC).
  const QVector4D clip_vec(2 * screen_x / p_->viewport_width - 1,
     2 * screen_y / p_->viewport_height - 1,
      2 * z - 1, 1);
  // NDC to world coordinates
  const QMatrix4x4 vp_inverse = GetViewProjectionMatrix().inverted();
  const QVector4D pworld = vp_inverse * clip_vec;
  // World coordinates to Cartesian world coordinates.
  const QVector3D world(pworld.x() / pworld.w(),
      pworld.y() / pworld.w(),
      pworld.z() / pworld.w());
  return world;
}

QMatrix4x4 CameraNode::GetProjectionMatrix() {
  return p_->projection_matrix;
}

QMatrix4x4 CameraNode::GetViewMatrix() {
  return WorldTransform().inverted();
}

QMatrix4x4 CameraNode::GetViewProjectionMatrix() {
  return p_->projection_matrix * GetViewMatrix();
}

const AxisAlignedBox& CameraNode::WorldBoundingBox() {
  return Priv::kBoundingBox;
}

void CameraNode::ComputeProjectionMatrix() {
  if (p_->proj_type == ProjectionType::kManual) {
    return;
  }

  p_->projection_matrix.fill(0.0f);
  if (p_->viewport_height <= 0 || p_->viewport_width <= 0 || p_->vfov_deg <= 0) {
    return;
  }

  const double aspect = static_cast<double>(p_->viewport_width) / p_->viewport_height;
  const double vfov = p_->vfov_deg * M_PI / 180;
  const double delta_z = p_->z_far - p_->z_near;

  switch (p_->proj_type) {
    case ProjectionType::kOrthographic:
      {
        const double dist_to_look_at = (Translation() - p_->look_at).length();
        const double bottom = dist_to_look_at * tan(vfov / 2);
        const double right = bottom * aspect;
        p_->projection_matrix(0, 0) = 1 / right;
        p_->projection_matrix(1, 1) = 1 / bottom;
        p_->projection_matrix(2, 2) = -2 / delta_z;
        p_->projection_matrix(2, 3) = -(p_->z_far + p_->z_near) / delta_z;
        p_->projection_matrix(3, 3) = 1;
      }
      break;
    case ProjectionType::kPerspective:
    default:
      {
        const double cotan = 1 / tan(vfov / 2);
        p_->projection_matrix(0, 0) = cotan / aspect;
        p_->projection_matrix(1, 1) = cotan;
        p_->projection_matrix(2, 2) = -(p_->z_far + p_->z_near) / delta_z;
        p_->projection_matrix(3, 2) = -1;
        p_->projection_matrix(2, 3) = -2 * p_->z_near * p_->z_far / delta_z;
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
  p_->look = QVector3D(-rot(0, 2), -rot(1, 2), -rot(2, 2));
  p_->up = QVector3D(rot(0, 1), rot(1, 1), rot(2, 1));
}

}  // namespace sv
