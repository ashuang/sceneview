// Copyright [2015] Albert Huang

#include "sceneview/view_handler_horizontal.hpp"

#include <cmath>
#include <iostream>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

#include "sceneview/camera_node.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/stock_resources.hpp"
#include "sceneview/viewport.hpp"

namespace sv {

struct ViewHandlerHorizontal::Priv {
  CameraNode* camera;

  Viewport* viewport;

  double mouse_speed;
  QVector3D zenith_dir;

  int first_mouse_x;
  int first_mouse_y;

  double movement_scale;
  QVector3D eye_start;
  QVector3D look_start;
  QVector3D up_start;

  bool show_look_at_point;
  DrawNode* look_at_shape;
  QTimer* hide_shape_timer;

  bool allow_azimuth_elevation_control;

  // Input handler widget
  QWidget* widget;
  QComboBox* projection_combo;
};
ViewHandlerHorizontal::ViewHandlerHorizontal(Viewport* viewport,
                                             const QVector3D& zenith_dir,
                                             QObject* parent)
    : QObject(parent) {
  p_ = new Priv();
  p_->camera = viewport->GetCamera();
  p_->viewport = viewport;
  p_->mouse_speed = 1.5;
  p_->zenith_dir = zenith_dir.normalized();
  p_->first_mouse_x = 0;
  p_->first_mouse_y = 0;
  p_->show_look_at_point = true;
  p_->look_at_shape = nullptr;
  p_->hide_shape_timer = nullptr;
  p_->allow_azimuth_elevation_control = true;
  p_->widget = nullptr;
}

ViewHandlerHorizontal::~ViewHandlerHorizontal() { delete p_; }

void ViewHandlerHorizontal::SetZenithDir(const QVector3D& dir) {
  p_->zenith_dir = dir.normalized();
}

const QVector3D& ViewHandlerHorizontal::ZenithDir() const {
  return p_->zenith_dir;
}

void ViewHandlerHorizontal::MousePressEvent(QMouseEvent* event) {
  p_->first_mouse_x = event->x();
  p_->first_mouse_y = event->y();

  const int cy = p_->viewport->height() / 2;
  const double vfov = p_->camera->GetVFovDeg() * M_PI / 180;
  const double vsize_at_pivot = PivotDistance() * tan(vfov / 2);
  const double vsize_per_pixel = vsize_at_pivot / cy;

  p_->movement_scale = vsize_per_pixel * p_->mouse_speed;

  p_->eye_start = p_->camera->Translation();
  p_->look_start = p_->camera->GetLookDir();
  p_->up_start = p_->camera->GetUpDir();

  UpdateShapeTransform();
}

void ViewHandlerHorizontal::MouseMoveEvent(QMouseEvent* event) {
  const int screen_dx = event->x() - p_->first_mouse_x;
  const int screen_dy = event->y() - p_->first_mouse_y;
  const Qt::MouseButtons buttons = event->buttons();

  if (buttons & Qt::LeftButton) {
    // X-Y pan
    const QVector3D left =
        QVector3D::crossProduct(p_->up_start, p_->look_start).normalized();
    const QVector3D forward =
        QVector3D::crossProduct(left, p_->zenith_dir).normalized();

    const double movement_left = screen_dx * p_->movement_scale;
    const double movement_forward = screen_dy * p_->movement_scale;

    const QVector3D new_eye =
        p_->eye_start + movement_left * left + movement_forward * forward;
    const QVector3D new_look_at = new_eye + p_->look_start * PivotDistance();

    p_->camera->LookAt(new_eye, new_look_at, p_->up_start);
    UpdateShapeTransform();
    p_->viewport->ScheduleRedraw();
  } else if (buttons & Qt::MidButton) {
    // Raise / lower the camera
    const QVector3D motion = screen_dy * p_->movement_scale * p_->zenith_dir;
    const QVector3D new_eye = p_->eye_start + motion;
    p_->camera->LookAt(new_eye, new_eye + p_->look_start * PivotDistance(),
                       p_->up_start);
    UpdateShapeTransform();
    p_->viewport->ScheduleRedraw();
  } else if (buttons & Qt::RightButton && p_->allow_azimuth_elevation_control) {
    // Rotate about the pivot
    const QVector3D look_at = p_->camera->GetLookAt();
    const QVector3D left =
        QVector3D::crossProduct(p_->up_start, p_->look_start).normalized();
    const double init_elevation =
        M_PI / 2 - acos(QVector3D::dotProduct(p_->look_start, p_->zenith_dir));

    double delevation = -screen_dy * 0.005;
    if (delevation + init_elevation < -M_PI / 2) {
      delevation = -M_PI / 2 - init_elevation;
    }
    if (delevation + init_elevation > M_PI / 2) {
      delevation = M_PI / 2 - init_elevation;
    }

    const QQuaternion elevation_rot =
        QQuaternion::fromAxisAndAngle(left, -delevation * 180 / M_PI);

    const double dazimuth = -screen_dx * 0.01;
    const QQuaternion azimuth_rot =
        QQuaternion::fromAxisAndAngle(p_->zenith_dir, dazimuth * 180 / M_PI);

    const QVector3D new_look =
        (azimuth_rot * elevation_rot).rotatedVector(p_->look_start);
    const QVector3D new_left = azimuth_rot.rotatedVector(left);

    const QVector3D new_eye = look_at - new_look * PivotDistance();
    const QVector3D new_up = QVector3D::crossProduct(new_look, new_left);

    p_->camera->LookAt(new_eye, look_at, new_up);
    UpdateShapeTransform();
    p_->viewport->ScheduleRedraw();
  }
}

void ViewHandlerHorizontal::WheelEvent(QWheelEvent* event) {
  if (event->buttons() != Qt::NoButton) {
    event->ignore();
    return;
  }
  // When the mouse wheel scrolls, move the camera so that it's closer or
  // farther from the look at point, without actually changing the look at
  // point.
  const double distance = PivotDistance();
  if ((event->delta() > 0 && distance < 1e-3) ||
      (event->delta() < 0 && distance > 1e7)) {
    p_->viewport->ScheduleRedraw();
    return;
  }
  const double new_distance = event->delta() * PivotDistance() * .001;

  const QVector3D new_eye =
      p_->camera->Translation() + p_->camera->GetLookDir() * new_distance;

  p_->camera->LookAt(new_eye, p_->camera->GetLookAt(), p_->camera->GetUpDir());
  UpdateShapeTransform();
  UpdateNearFarPlanes();
  p_->viewport->ScheduleRedraw();
}

void ViewHandlerHorizontal::KeyPressEvent(QKeyEvent* event) {
  int direction = 0;
  if (event->key() == Qt::Key_Up) {
    direction = 1;
  } else if (event->key() == Qt::Key_Down) {
    direction = -1;
  } else {
    event->ignore();
  }

  const int cy = p_->viewport->height() / 2;
  const double vfov = p_->camera->GetVFovDeg() * M_PI / 180;
  const double vsize_at_pivot = PivotDistance() * tan(vfov / 2);
  const double vsize_per_pixel = vsize_at_pivot / cy;
  const QVector3D motion = vsize_per_pixel * p_->zenith_dir * direction * 10;
  const QVector3D new_eye = p_->camera->Translation() + motion;
  const QVector3D new_look_at = p_->camera->GetLookAt() + motion;
  p_->camera->LookAt(new_eye, new_look_at, p_->camera->GetUpDir());
  UpdateShapeTransform();
  p_->viewport->ScheduleRedraw();
}

QWidget* ViewHandlerHorizontal::GetWidget() {
  if (p_->widget) {
    return p_->widget;
  }

  // Setup the widget
  p_->widget = new QWidget();
  QGridLayout* layout = new QGridLayout(p_->widget);

  p_->widget->setMinimumSize(200, 50);
  layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  layout->setSpacing(0);
  layout->setContentsMargins(2, 2, 2, 2);

  int grid_row = 0;

  // Combo box to select the projection type
  layout->addWidget(new QLabel("Projection"), grid_row, 0);

  p_->projection_combo = new QComboBox(p_->widget);
  p_->projection_combo->addItem("Perspective", CameraNode::kPerspective);
  p_->projection_combo->addItem("Orthographic", CameraNode::kOrthographic);
  layout->addWidget(p_->projection_combo, grid_row, 1);
  connect(p_->projection_combo, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(OnProjectionSelectionChanged()));

  grid_row++;

  // When the viewport's active camera is changed externally, update the widgets
  // to match the camera's settings.
  connect(p_->viewport, &Viewport::CameraChanged, [this](CameraNode* camera) {
    const int proj_combo_index =
        p_->projection_combo->findData(camera->GetProjectionType());
    p_->projection_combo->setCurrentIndex(proj_combo_index);
  });

  return p_->widget;
}

void ViewHandlerHorizontal::SetShowLookAtPoint(bool val) {
  p_->show_look_at_point = val;
}

void ViewHandlerHorizontal::SetAllowAzimuthElevationControl(bool val) {
  p_->allow_azimuth_elevation_control = val;
}

double ViewHandlerHorizontal::PivotDistance() const {
  return (p_->camera->Translation() - p_->camera->GetLookAt()).length();
}

void ViewHandlerHorizontal::MakeShape() {
  Scene::Ptr scene = p_->viewport->GetScene();
  ResourceManager::Ptr resources = p_->viewport->GetResources();
  GroupNode* root = scene->Root();
  StockResources stock(resources);

  MaterialResource::Ptr material =
      stock.NewMaterial(StockResources::kUniformColorLighting);
  material->SetParam("diffuse", 0, 1.0, 0, 1.0);

  p_->look_at_shape = scene->MakeDrawNode(root, "vhz:shape");
  p_->look_at_shape->Add(stock.Sphere(), material);
  p_->look_at_shape->SetVisible(false);

  const QVector3D unitz(0, 0, 1);
  const QVector3D axis =
      QVector3D::crossProduct(unitz, p_->zenith_dir).normalized();
  const double cosangle = QVector3D::dotProduct(unitz, p_->zenith_dir);
  const double angle_deg = acos(cosangle) * 180 / M_PI;
  QQuaternion quat = QQuaternion::fromAxisAndAngle(axis, angle_deg);
  p_->look_at_shape->SetRotation(quat);

  p_->hide_shape_timer = new QTimer(this);
  p_->hide_shape_timer->setSingleShot(true);
  connect(p_->hide_shape_timer, &QTimer::timeout, [this]() {
    p_->look_at_shape->SetVisible(false);
    p_->viewport->ScheduleRedraw();
  });
}

void ViewHandlerHorizontal::UpdateShapeTransform() {
  if (!p_->show_look_at_point) {
    return;
  }
  if (!p_->look_at_shape) {
    MakeShape();
  }
  p_->look_at_shape->SetVisible(true);

  const double dscale = PivotDistance() * 0.05;
  p_->look_at_shape->SetScale(dscale, dscale, 0.2 * dscale);
  p_->look_at_shape->SetTranslation(p_->camera->GetLookAt());

  if (p_->hide_shape_timer->isActive()) {
    p_->hide_shape_timer->stop();
  }

  p_->hide_shape_timer->start(500);
}

void ViewHandlerHorizontal::UpdateNearFarPlanes() {
  CameraNode* camera = p_->viewport->GetCamera();
  const double distance = PivotDistance();
  const double z_near = distance / 100;
  const double z_far = distance * 1000;
  const double vfov_deg = camera->GetVFovDeg();
  switch (camera->GetProjectionType()) {
    case CameraNode::kPerspective:
      camera->SetPerspective(vfov_deg, z_near, z_far);
      break;
    case CameraNode::kOrthographic:
      camera->SetOrthographic(vfov_deg, z_near, z_far);
      break;
    default:
      break;
  }
}

void ViewHandlerHorizontal::OnProjectionSelectionChanged() {
  CameraNode* camera = p_->viewport->GetCamera();

  const double z_near = camera->GetZNear();
  const double z_far = camera->GetZFar();
  const double vfov_deg = camera->GetVFovDeg();
  CameraNode::ProjectionType proj_type =
      static_cast<CameraNode::ProjectionType>(
          p_->projection_combo->currentData().toInt());
  switch (proj_type) {
    case CameraNode::kPerspective:
      camera->SetPerspective(vfov_deg, z_near, z_far);
      break;
    case CameraNode::kOrthographic:
      camera->SetOrthographic(vfov_deg, z_near, z_far);
      break;
    default:
      break;
  }
  p_->viewport->ScheduleRedraw();
}

}  // namespace sv
