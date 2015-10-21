// Input vertex position (model space)
attribute vec4 sv_vert_pos;

// Vertex color
attribute vec4 sv_diffuse;

// Model-view-projection matrix
uniform mat4 sv_mvp_mat;

varying vec4 color;

void main(void)
{
  color = sv_diffuse;
  gl_Position = sv_mvp_mat * sv_vert_pos;
}
