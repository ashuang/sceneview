#include "fixed_function_renderer.hpp"

#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#endif

namespace sv = sceneview;
using sv::Renderer;

namespace vis_examples {

FixedFunctionRenderer::FixedFunctionRenderer(const QString& name,
    QObject* parent) :
  Renderer(name, parent),
  counter_(),
  angle_(0) {
  counter_.start();
}

/**
 * Called before the scene graph is rendered.
 *
 * When this function is called, the following is true:
 * - No shader programs are active (i.e., glUseProgram(0) has been called)
 * - The projection matrix (GL_PROJECTION) is setup according to the active
 *   camera.
 * - The modelview matrix stack has a single matrix on it that correspnods to
 *   the camera view matrix.
 * - The GL_MODELVIEW matrix stack is active.
 *
 * In other words, the matrix stack is setup such that you can render in
 * "world" coordinates.
 *
 * You can issue legacy fixed-function OpenGL commands (e.g., glColor3f(),
 * glBegin(), glVertex3f(), etc. calls)
 *
 * You can also use your own vertex and fragment shaders.
 */
void FixedFunctionRenderer::RenderBegin() {
}

/**
 * Called after the scene graph is rendered.
 *
 * This function has the same guarantees as RenderBegin().
 */
void FixedFunctionRenderer::RenderEnd() {
  const double elapsed = counter_.restart() / 1000.;
  const double speed = 0.1;
  angle_ += elapsed * speed;
  const double orbit_radius = 5.0;
  const float x = orbit_radius * cos(angle_);
  const float y = orbit_radius * sin(angle_);
  glPushMatrix();
  glTranslatef(x, y, 0);

  glEnable(GL_LIGHTING);
  const float ambient[4] = { 0.1, 0, 0, 1 };
  const float diffuse[4] = { 1, 0, 0, 1 };
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

  const sv::GeometryData geom = sv::StockResources::SphereData();
  glBegin(geom.gl_mode);
  for (int index : geom.indices) {
    const QVector3D& normal = geom.normals[index];
    const QVector3D& vertex = geom.vertices[index];
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(vertex.x(), vertex.y(), vertex.z());
  }
  glEnd();

  glPopMatrix();
}

}  // namespace vis_examples
