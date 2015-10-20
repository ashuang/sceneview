// Input vertex position
attribute highp vec4 b3_vert_pos;

// Model-view matrix
uniform mediump mat4 b3_mv_mat;

// Projection matrix
uniform mediump mat4 b3_proj_mat;

void main(void)
{
  vec4 translation = vec4(b3_mv_mat[3][0],
      b3_mv_mat[3][1],
      b3_mv_mat[3][2],
      0);
    gl_Position = b3_proj_mat * (translation + b3_vert_pos);
}
