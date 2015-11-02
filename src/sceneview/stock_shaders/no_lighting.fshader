// No lighting model.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    COLOR_PER_VERTEX
//    COLOR_UNIFORM

#ifdef COLOR_UNIFORM
uniform vec4 color;
#endif

#ifdef COLOR_PER_VERTEX
varying vec4 color;
#endif

void main(void) {
  gl_FragColor = color;
}
