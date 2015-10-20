// Copyright [2015] Albert Huang

#ifndef INTERNAL_GL_H__
#define INTERNAL_GL_H__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <string>

namespace sceneview {

const char* glErrorString(GLenum error);

}

#endif  // INTERNAL_GL_H__
