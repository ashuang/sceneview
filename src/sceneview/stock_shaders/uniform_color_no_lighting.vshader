// Input vertex position (model space)
attribute highp vec4 sv_vert_pos;

// Model-view-projection matrix
uniform mediump mat4 sv_mvp_mat;

void main(void)
{
    gl_Position = sv_mvp_mat * sv_vert_pos;
}
