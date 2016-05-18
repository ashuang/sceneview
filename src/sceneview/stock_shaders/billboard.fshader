// Billboard shader.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    USE_TEXTURE0
//    COLOR_UNIFORM


#ifdef USE_TEXTURE0
varying mediump vec2 texc_0;

uniform sampler2D texture0;

uniform vec4 text_color;
#endif

#ifdef COLOR_UNIFORM
uniform vec4 color;
#endif

void main(void) {
#ifdef USE_TEXTURE0
  vec4 color = texture2D(texture0, texc_0) * text_color;
  if (color.a < 0.1)
    discard;
#endif
  gl_FragColor = color;
}
