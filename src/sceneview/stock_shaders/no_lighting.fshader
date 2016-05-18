// No lighting model.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    COLOR_PER_VERTEX
//    COLOR_UNIFORM
//
// USE_TEXTURE0 can also be defined to use a texture

#ifdef COLOR_UNIFORM
uniform vec4 color;
#endif

#ifdef COLOR_PER_VERTEX
varying vec4 color;
#endif

#ifdef USE_TEXTURE0
varying mediump vec2 texc_0;
uniform sampler2D texture0;
#endif

void main(void) {
#ifdef USE_TEXTURE0
  vec4 frag_color = texture2D(texture0, texc_0) * color;
  if (frag_color.a < 0.1)
    discard;
  gl_FragColor = frag_color;
#else
  gl_FragColor = color;
#endif
}
