// Billboard shader.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    COLOR_TEXTURE
//    COLOR_UNIFORM


#ifdef COLOR_TEXTURE
varying mediump vec2 texc;

uniform sampler2D texture;

uniform vec4 text_color;
#endif

#ifdef COLOR_UNIFORM
uniform vec4 color;
#endif

void main(void) {
#ifdef COLOR_TEXTURE
  vec4 color = texture2D(texture, texc) * text_color;
  if (color.a < 0.1)
    discard;
#endif
  gl_FragColor = color;
}
