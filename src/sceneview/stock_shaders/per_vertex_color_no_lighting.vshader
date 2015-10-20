// Input vertex position (model space)
attribute vec4 b3_vert_pos;

// Vertex color
attribute vec4 b3_diffuse;

// Model-view-projection matrix
uniform mat4 b3_mvp_mat;

varying vec4 color;

void main(void)
{
  color = b3_diffuse;
  gl_Position = b3_mvp_mat * b3_vert_pos;
}
