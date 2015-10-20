varying mediump vec2 texc;

uniform sampler2D texture;

uniform vec4 text_color;

void main(void) {
  vec4 color = texture2D(texture, texc) * text_color;
  if (color.a < 0.1)
    discard;
  gl_FragColor = color;
}
