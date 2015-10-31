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

ViewHandlerHorizontal::ViewHandlerHorizontal(Viewport* viewport,
    const QVector3D& zenith_dir,
    QObject* parent) :
  QObject(parent),
  camera_(viewport->GetCamera()),
  viewport_(viewport),
  mouse_speed_(1.5),
  zenith_dir_(zenith_dir.normalized()),
  first_mouse_x_(0),
  first_mouse_y_(0),
  show_look_at_point_(true),
  look_at_shape_(nullptr),
  hide_shape_timer_(nullptr),
  allow_azimuth_elevation_control_(true),
  widget_(nullptr) {
}

void ViewHandlerHorizontal::SetZenithDir(const QVector3D& dir) {
  zenith_dir_ = dir.normalized();
}

void ViewHandlerHorizontal::MousePressEvent(QMouseEvent *event) {
  first_mouse_x_ = event->x();
  first_mouse_y_ = event->y();

  const int cy = viewport_->height() / 2;
  const double vfov = camera_->GetVFovDeg() * M_PI / 180;
  const double vsize_at_pivot = PivotDistance() * tan(vfov / 2);
  const double vsize_per_pixel = vsize_at_pivot / cy;

  movement_scale_ = vsize_per_pixel * mouse_speed_;

  eye_start_ = camera_->Translation();
  look_start_ = camera_->GetLookDir();
  up_start_ = camera_->GetUpDir();

  UpdateShapeTransform();
}

void ViewHandlerHorizontal::MouseMoveEvent(QMouseEvent *event) {
  const int screen_dx = event->x() - first_mouse_x_;
  const int screen_dy = event->y() - first_mouse_y_;
  const Qt::MouseButtons buttons = event->buttons();

  if (buttons & Qt::LeftButton) {
    // X-Y pan
    const QVector3D left = QVector3D::crossProduct(
        up_start_, look_start_).normalized();
    const QVector3D forward = QVector3D::crossProduct(
        left, zenith_dir_).normalized();

    const double movement_left = screen_dx * movement_scale_;
    const double movement_forward = screen_dy * movement_scale_;

    const QVector3D new_eye = eye_start_ +
      movement_left * left +
      movement_forward * forward;
    const QVector3D new_look_at = new_eye + look_start_ * PivotDistance();

    camera_->LookAt(new_eye, new_look_at, up_start_);
    UpdateShapeTransform();
    viewport_->ScheduleRedraw();
  } else if (buttons & Qt::MidButton) {
    // Raise / lower the camera
    const QVector3D motion = screen_dy * movement_scale_ * zenith_dir_;
    const QVector3D new_eye = eye_start_ + motion;
    camera_->LookAt(new_eye, new_eye + look_start_ * PivotDistance(),
        up_start_);
    UpdateShapeTransform();
    viewport_->ScheduleRedraw();
  } else if (buttons & Qt::RightButton && allow_azimuth_elevation_control_) {
    // Rotate about the pivot
    const QVector3D look_at = camera_->GetLookAt();
    const QVector3D left = QVector3D::crossProduct(
        up_start_, look_start_).normalized();
    const double init_elevation = M_PI / 2 -
      acos(QVector3D::dotProduct(look_start_, zenith_dir_));

    double delevation = -screen_dy * 0.005;
    if (delevation + init_elevation < -M_PI/2) {
      delevation = -M_PI/2 - init_elevation;
    }
    if (delevation + init_elevation > M_PI/2) {
      delevation = M_PI/2 - init_elevation;
    }

    const QQuaternion elevation_rot = QQuaternion::fromAxisAndAngle(
        left, -delevation * 180 / M_PI);

    const double dazimuth = -screen_dx * 0.01;
    const QQuaternion azimuth_rot = QQuaternion::fromAxisAndAngle(
        zenith_dir_, dazimuth * 180 / M_PI);

    const QVector3D new_look = (azimuth_rot * elevation_rot).rotatedVector(
        look_start_);
    const QVector3D new_left = azimuth_rot.rotatedVector(left);

    const QVector3D new_eye = look_at - new_look * PivotDistance();
    const QVector3D new_up = QVector3D::crossProduct(new_look, new_left);

    camera_->LookAt(new_eye, look_at, new_up);
    UpdateShapeTransform();
    viewport_->ScheduleRedraw();
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
    viewport_->ScheduleRedraw();
    return;
  }
  const double new_distance = event->delta() * PivotDistance() * .001;

  const QVector3D new_eye = camera_->Translation() + camera_->GetLookDir() * new_distance;

  camera_->LookAt(new_eye, camera_->GetLookAt(), camera_->GetUpDir());
  UpdateShapeTransform();
  UpdateNearFarPlanes();
  viewport_->ScheduleRedraw();
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

  const int cy = viewport_->height() / 2;
  const double vfov = camera_->GetVFovDeg() * M_PI / 180;
  const double vsize_at_pivot = PivotDistance() * tan(vfov / 2);
  const double vsize_per_pixel = vsize_at_pivot / cy;
  const QVector3D motion = vsize_per_pixel * zenith_dir_ * direction * 10;
  const QVector3D new_eye = camera_->Translation() + motion;
  const QVector3D new_look_at = camera_->GetLookAt() + motion;
  camera_->LookAt(new_eye, new_look_at, camera_->GetUpDir());
  UpdateShapeTransform();
  viewport_->ScheduleRedraw();
}

QWidget* ViewHandlerHorizontal::GetWidget() {
  if (widget_) {
    return widget_;
  }

  // Setup the widget
  widget_ = new QWidget();
  QGridLayout* layout = new QGridLayout(widget_);

  widget_->setMinimumSize(200, 50);
  layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  layout->setSpacing(0);
  layout->setContentsMargins(2, 2, 2, 2);

  int grid_row = 0;

  // Combo box to select the projection type
  layout->addWidget(new QLabel("Projection"), grid_row, 0);

  projection_combo_ = new QComboBox(widget_);
  projection_combo_->addItem("Perspective", CameraNode::kPerspective);
  projection_combo_->addItem("Orthographic", CameraNode::kOrthographic);
  layout->addWidget(projection_combo_, grid_row, 1);
  connect(projection_combo_, SIGNAL(currentIndexChanged(const QString&)),
      this, SLOT(OnProjectionSelectionChanged()));

  grid_row++;

  // When the viewport's active camera is changed externally, update the widgets
  // to match the camera's settings.
  connect(viewport_, &Viewport::CameraChanged,
      [this](CameraNode* camera) {
      const int proj_combo_index =
        projection_combo_->findData(camera->GetProjectionType());
      projection_combo_->setCurrentIndex(proj_combo_index);
    });

  return widget_;
}

void ViewHandlerHorizontal::SetShowLookAtPoint(bool val) {
  show_look_at_point_ = val;
}

void ViewHandlerHorizontal::SetAllowAzimuthElevationControl(bool val) {
  allow_azimuth_elevation_control_ = val;
}

double ViewHandlerHorizontal::PivotDistance() const {
  return (camera_->Translation() - camera_->GetLookAt()).length();
}

void ViewHandlerHorizontal::MakeShape() {
  Scene::Ptr scene = viewport_->GetScene();
  ResourceManager::Ptr resources = viewport_->GetResources();
  GroupNode* root = scene->Root();
  StockResources stock(resources);

  MaterialResource::Ptr material =
    stock.NewMaterial(StockResources::kUniformColorLighting);
  material->SetParam("diffuse", 0, 1.0, 0, 1.0);

  look_at_shape_ = scene->MakeDrawNode(root, "vhz:shape");
  look_at_shape_->Add(stock.Sphere(), material);
  look_at_shape_->SetVisible(false);

  const QVector3D unitz(0, 0, 1);
  const QVector3D axis = QVector3D::crossProduct(
      unitz, zenith_dir_).normalized();
  const double cosangle = QVector3D::dotProduct(unitz, zenith_dir_);
  const double angle_deg = acos(cosangle) * 180 / M_PI;
  QQuaternion quat = QQuaternion::fromAxisAndAngle(axis, angle_deg);
  look_at_shape_->SetRotation(quat);

  hide_shape_timer_ = new QTimer(this);
  hide_shape_timer_->setSingleShot(true);
  connect(hide_shape_timer_, &QTimer::timeout,
      [this](){
        look_at_shape_->SetVisible(false);
        viewport_->ScheduleRedraw();
      });
}

void ViewHandlerHorizontal::UpdateShapeTransform() {
  if (!show_look_at_point_) {
    return;
  }
  if (!look_at_shape_) {
    MakeShape();
  }
  look_at_shape_->SetVisible(true);

  const double dscale = PivotDistance() * 0.05;
  look_at_shape_->SetScale(dscale, dscale, 0.2 * dscale);
  look_at_shape_->SetTranslation(camera_->GetLookAt());

  if (hide_shape_timer_->isActive()) {
    hide_shape_timer_->stop();
  }

  hide_shape_timer_->start(500);
}

void ViewHandlerHorizontal::UpdateNearFarPlanes() {
  CameraNode* camera = viewport_->GetCamera();
  const double distance = PivotDistance();
  const double z_near = distance / 100;
  const double z_far = distance * 1000;
  const double vfov_deg = camera->GetVFovDeg();
  const CameraNode::ProjectionType proj_type = camera->GetProjectionType();
  camera->SetProjectionParams(proj_type, vfov_deg, z_near, z_far);
}

void ViewHandlerHorizontal::OnProjectionSelectionChanged() {
  CameraNode* camera = viewport_->GetCamera();

  const double z_near = camera->GetZNear();
  const double z_far = camera->GetZFar();
  const double vfov_deg = camera->GetVFovDeg();
  CameraNode::ProjectionType proj_type =
    static_cast<CameraNode::ProjectionType>(
        projection_combo_->currentData().toInt());
  camera->SetProjectionParams(proj_type, vfov_deg, z_near, z_far);
  viewport_->ScheduleRedraw();
}

}  // namespace sv
