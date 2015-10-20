#include "internal_gl.h"
#include <sceneview/viewport.hpp>

#include <iostream>
#include <QOpenGLFunctions>
#include <QTimer>

#include <sceneview/camera_node.hpp>
#include <sceneview/draw_scene.hpp>
#include <sceneview/input_handler.hpp>
#include <sceneview/light_node.hpp>
#include <sceneview/renderer.hpp>

namespace sceneview {

Viewport::Viewport(const ResourceManager::Ptr& resources,
    const Scene::Ptr& scene, QWidget* parent) :
  QOpenGLWidget(parent),
  resources_(resources),
  scene_(scene),
  camera_(nullptr),
  input_handler_(nullptr),
  draw_(new DrawScene(resources_, scene)),
  renderers_(),
  input_handlers_(),
  redraw_scheduled_(false),
  m_context_(nullptr),
  background_color_(0, 0, 0, 1) {

  // Enable multisampling so that things draw a little smoother.
  QSurfaceFormat format = QSurfaceFormat::defaultFormat();
  format.setSamples(2);
  setFormat(format);
}

Viewport::~Viewport() {
  // Activate the opengl context and then shut down the renderers.
  makeCurrent();
  if (input_handler_) {
    input_handler_->Deactivated();
  }
  for (Renderer* renderer : renderers_) {
    renderer->ShutdownGL();
  }
  for (InputHandler* handler : input_handlers_) {
    handler->ShutdownGL();
  }
  renderers_.clear();
  m_context_ = nullptr;
}

void Viewport::AddRenderer(Renderer* renderer) {
  renderers_.push_back(renderer);
  renderer->SetViewport(this);
  renderer->SetBaseNode(scene_->MakeGroup(scene_->Root(),
        "basenode_" + renderer->Name()));

  if (m_context_) {
    makeCurrent();
    renderer->InitializeGL();
  }

  emit RendererAdded(renderer);
}

void Viewport::AddInputHandler(InputHandler* handler) {
  input_handlers_.push_back(handler);
  if (m_context_) {
    handler->InitializeGL();
  }
  emit InputHandlerAdded(handler);

  // If it's the only input handers, activate it.
  if (input_handlers_.size() == 1) {
    ActivateInputHandler(handler);
  }
}

void Viewport::SetCamera(CameraNode* camera_node) {
  if (camera_ == camera_node) {
    return;
  }
  if (!scene_->ContainsNode(camera_node)) {
    throw std::invalid_argument("camera doesn't belong the scene");
  }
  camera_ = camera_node;
  camera_->SetViewportSize(width(), height());

  emit CameraChanged(camera_);
}

void Viewport::ScheduleRedraw() {
  if (!redraw_scheduled_) {
    QTimer::singleShot(30, this, SLOT(Render()));
    redraw_scheduled_ = true;
  }
}

void Viewport::ActivateInputHandler(InputHandler* handler) {
  if (input_handler_ == handler) {
    return;
  }
  if (input_handler_) {
    input_handler_->Deactivated();
  }
  input_handler_ = handler;
  input_handler_->Activated();
  emit InputHandlerActivated(handler);
}

void Viewport::initializeGL() {
  m_context_ = QOpenGLContext::currentContext();

  for (Renderer* renderer : renderers_) {
    renderer->InitializeGL();
  }

  for (InputHandler* handler : input_handlers_) {
    handler->InitializeGL();
  }
}

void Viewport::resizeGL(int width, int height) {
  if (camera_) {
    camera_->SetViewportSize(width, height);
  }
}

static void CheckGLErrors(const QString& name) {
  GLenum err_code = glGetError ();
  const char *err_str;
  while (err_code != GL_NO_ERROR) {
    err_str = sceneview::glErrorString(err_code);
    fprintf(stderr, "OpenGL Error (%s)\n", name.toStdString().c_str());
    fprintf(stderr, "%s\n", err_str);
    err_code = glGetError();
  }
}

void Viewport::paintGL() {
  redraw_scheduled_ = false;

  QOpenGLFunctions* gl = m_context_->functions();

  // Clear the drawing area
  gl->glClearColor(background_color_.redF(),
      background_color_.greenF(),
      background_color_.blueF(),
      background_color_.alphaF());
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Setup the fixed-function pipeline.
  PrepareFixedFunctionPipeline();

  // Inform the renderers that drawing is about to begin
  for (Renderer* renderer : renderers_) {
    if (renderer->Enabled()) {
      glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_POLYGON_STIPPLE_BIT |
          GL_POLYGON_BIT | GL_LINE_BIT | GL_FOG_BIT | GL_LIGHTING_BIT);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      renderer->RenderBegin();
      CheckGLErrors(renderer->Name());
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPopAttrib();
    }
  }

  // Draw the scene
  draw_->Draw(camera_);

  // Setup the fixed-function pipeline again.
  PrepareFixedFunctionPipeline();

  // Notify renderers that drawing has finished
  for (Renderer* renderer : renderers_) {
    if (renderer->Enabled()) {
      glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_POLYGON_STIPPLE_BIT |
          GL_POLYGON_BIT | GL_LINE_BIT | GL_FOG_BIT | GL_LIGHTING_BIT);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      renderer->RenderEnd();
      CheckGLErrors(renderer->Name());
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPopAttrib();
    }
  }

  // All done drawing.
  m_context_->swapBuffers(m_context_->surface());
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->MousePressEvent(event);
  }
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->MouseMoveEvent(event);
  }
}

void Viewport::mouseReleaseEvent(QMouseEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->MouseReleaseEvent(event);
  }
}

void Viewport::wheelEvent(QWheelEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->WheelEvent(event);
  }
}

void Viewport::keyPressEvent(QKeyEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->KeyPressEvent(event);
  }
}

void Viewport::keyReleaseEvent(QKeyEvent* event) {
  if (input_handler_) {
    makeCurrent();
    input_handler_->KeyReleaseEvent(event);
  }
}

void Viewport::Render() {
  repaint(0, 0, width(), height());
}

void Viewport::PrepareFixedFunctionPipeline() {
  // Enable the fixed function pipeline by disabling any active shader program.
  glUseProgram(0);

  // Setup the projection and view matrices
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMultMatrixf(camera_->GetProjectionMatrix().constData());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(camera_->GetViewMatrix().constData());

  // Setup lights
  const GLenum gl_lights[] = {
    GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
    GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7
  };
  std::vector<LightNode*> lights = scene_->Lights();
  for (int light_ind = 0; light_ind < 8; ++light_ind) {
    const GLenum gl_light = gl_lights[light_ind];
    LightNode* light = lights[light_ind];
    const LightType light_type = light->GetLightType();

    if (light_type == LightType::kDirectional) {
      const QVector3D dir = light->Direction();
      const float dir4f[4] = { dir.x(), dir.y(), dir.z(), 0 };
      glLightfv(gl_light, GL_POSITION, dir4f);
    } else {
      const QVector3D posf = light->Translation();
      const float pos4f[4] = { posf.x(), posf.y(), posf.z(), 1};
      glLightfv(gl_light, GL_POSITION, pos4f);

      const float attenuation = light->Attenuation();
      glLightf(gl_light, GL_QUADRATIC_ATTENUATION, attenuation);
      glLightf(gl_light, GL_CONSTANT_ATTENUATION, 1.0);

      if (light_type == LightType::kSpot) {
        const float cone_angle_deg = light->ConeAngle();
        glLightf(gl_light, GL_SPOT_CUTOFF, cone_angle_deg);
        glLightf(gl_light, GL_SPOT_EXPONENT, 1.2);
      }
    }

    const QVector3D& color = light->Color();
    const QVector3D& ambient = color * light->Ambient();
    const float color4f[4] = { color.x(),  color.y(),  color.z(), 1 };
    const float ambient4f[4] = { ambient.x(),  ambient.y(),  ambient.z(), 1 };
    glLightfv(gl_light, GL_AMBIENT, ambient4f);
    glLightfv(gl_light, GL_DIFFUSE, color4f);
    glLightfv(gl_light, GL_SPECULAR, color4f);

    glEnable(gl_light);
    break;
  }

  // Set some default rendering parameters
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

}  // namespace sceneview
