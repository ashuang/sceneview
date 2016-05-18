// Stock lighting model.
// Before compiling, one of the following must be #defined and prepended to
// this program:
//    COLOR_PER_VERTEX
//    COLOR_UNIFORM
//
// USE_TEXTURE0 can also be defined to use a texture.

// View matrix inverse
uniform mat4 sv_view_mat_inv;

// Model matrix
#define B3_MAX_LIGHTS 4
uniform struct Light {
  bool is_directional;
  vec3 position;
  vec3 direction;
  vec3 color;
  float ambient;
  float specular;
  float attenuation;
  float cone_angle;
} sv_lights[B3_MAX_LIGHTS];

#ifdef COLOR_UNIFORM
uniform float shininess;
uniform vec4 diffuse;
uniform vec4 specular;
#endif
#ifdef COLOR_PER_VERTEX
varying float shininess;
varying vec4 diffuse;
varying vec4 specular;
#endif
#ifdef USE_TEXTURE0
varying mediump vec2 texc_0;
uniform sampler2D texture0;
vec4 diffuse_tex_color;
#endif

varying vec3 normal;
varying vec3 surface_pos;

vec4 LightContribution(Light light, vec3 surface_pos, vec3 eye_pos,
    vec3 surface_to_eye, vec3 normal) {
  // All calculations done in world space
  vec3 surface_to_light;
  float attenuation = 1.0;

  if (light.is_directional) {
    surface_to_light = normalize(light.direction);
  } else {
    surface_to_light = light.position - surface_pos;
    float distance_to_light = length(surface_to_light);
    surface_to_light = normalize(surface_to_light);
    attenuation = 1.0 / (1.0 + light.attenuation * pow(distance_to_light, 2.0));

    // Calculate spotlight cutoff / attenuation
    float spot_dot = dot(-surface_to_light, light.direction);
    if (spot_dot < cos(light.cone_angle)) {
      attenuation = 0.0;
    } else {
      attenuation *= pow(spot_dot, 1.2);
    }
  }

  // Apply a variation on hemisphere lighting to get both diffuse and ambient
  // components. This provides a smoother falloff in brightness when the
  // surface faces away from the light, as opposed to adding a constant ambient
  // factor.
  float diffuse_front = dot(normal, surface_to_light) * 0.5 + 0.5;
  float diffuse_coeff = (1.0 - light.ambient) * diffuse_front + light.ambient;
  vec3 diffuse_k = diffuse_coeff * light.color * attenuation;
#ifdef USE_TEXTURE0
  vec3 diffuse_term = diffuse_k * diffuse_tex_color.rgb;
#else
  vec3 diffuse_term = diffuse_k * diffuse.rgb;
#endif
  diffuse_term = clamp(diffuse_term, 0.0, 1.0);

  // Blinn-Phong model for specularities
  vec3 half_dir = normalize(surface_to_light + surface_to_eye);
  float specular_coeff = pow(max(0.0, dot(half_dir, normal)), shininess);

  // Phong shading
//  vec3 reflection = normalize(-reflect(-surface_to_light, normal));
//  float specular_coeff = pow(max(0.0, dot(reflection, surface_to_eye)), 0.3 * shininess);

  vec3 specular_term = specular_coeff * light.color * light.specular * specular.rgb * attenuation;
  specular_term = clamp(specular_term, 0.0, 1.0);

  return vec4(diffuse_term, diffuse.a) +
         vec4(specular_term, specular.a);
}

void main(void) {
  vec3 eye_pos = sv_view_mat_inv[2].xyz;
  vec3 surface_to_eye = normalize(eye_pos - surface_pos);

#ifdef USE_TEXTURE0
  diffuse_tex_color = texture2D(texture0, texc_0);
#endif

  vec4 color = vec4(0);
  for (int light_ind = 0; light_ind < B3_MAX_LIGHTS; ++light_ind) {
    color += LightContribution(sv_lights[light_ind],
        surface_pos, eye_pos, surface_to_eye, normal);
  }

  gl_FragColor = color;
}

// vim: ft=glsl
