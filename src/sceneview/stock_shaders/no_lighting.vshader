// No lighting model.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    COLOR_PER_VERTEX
//    COLOR_UNIFORM

// Input vertex position (model space)
attribute vec4 sv_vert_pos;

// Model-view-projection matrix
uniform mat4 sv_mvp_mat;

#ifdef COLOR_PER_VERTEX
// Vertex color
attribute vec4 sv_diffuse;

varying vec4 color;
#endif

void main(void)
{
#ifdef COLOR_PER_VERTEX
  color = sv_diffuse;
#endif

  gl_Position = sv_mvp_mat * sv_vert_pos;
}
