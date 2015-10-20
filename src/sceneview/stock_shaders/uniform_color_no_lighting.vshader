// Input vertex position (model space)
attribute highp vec4 b3_vert_pos;

// Model-view-projection matrix
uniform mediump mat4 b3_mvp_mat;

void main(void)
{
    gl_Position = b3_mvp_mat * b3_vert_pos;
}
