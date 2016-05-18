// Billboard shader.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    USE_TEXTURE0
//    COLOR_UNIFORM

// Input vertex position
attribute highp vec4 sv_vert_pos;

// Model-view matrix
uniform mediump mat4 sv_mv_mat;

// Projection matrix
uniform mediump mat4 sv_proj_mat;

#ifdef USE_TEXTURE0
// Texture coordinates
attribute mediump vec2 sv_tex_coords_0;

varying mediump vec2 texc_0;
#endif

void main(void)
{
  vec4 translation = vec4(sv_mv_mat[3][0],
      sv_mv_mat[3][1],
      sv_mv_mat[3][2],
      0);
  gl_Position = sv_proj_mat * (translation + sv_vert_pos);

#ifdef USE_TEXTURE0
  texc_0 = sv_tex_coords_0;
#endif
}
