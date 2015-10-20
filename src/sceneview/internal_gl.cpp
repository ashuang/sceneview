#include "internal_gl.h"

namespace sceneview {

const char* glErrorString(GLenum error) {
  switch(error) {
    case GL_NO_ERROR:
      return "No error";
    case GL_INVALID_ENUM:
      return "Invalid enum";
    case GL_INVALID_VALUE:
      return "Invalid value";
    case GL_INVALID_OPERATION:
      return "Invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "Invalid framebuffer operation";
    case GL_OUT_OF_MEMORY:
      return "Out of memory";
    case GL_STACK_UNDERFLOW:
      return "Stack underflow";
    case GL_STACK_OVERFLOW:
      return "Stack overflow";
    default:
      return "Unknown error";
  }
}

}  // namespace sceneview
