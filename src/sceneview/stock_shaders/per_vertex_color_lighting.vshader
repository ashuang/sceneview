// Input vertex position (model space)
attribute vec4 b3_vert_pos;

// Input vertex normal vector
attribute vec3 b3_normal;

attribute float b3_shininess;
attribute vec4 b3_diffuse;
attribute vec4 b3_specular;

#define B3_MAX_LIGHTS 4
uniform struct Light {
  bool is_directional;
  vec3 position;
  vec3 direction;
  vec3 color;
  float ambient;
  float attenuation;
  float cone_angle;
} b3_lights[B3_MAX_LIGHTS];

// Model-view-projection matrix
uniform mat4 b3_mvp_mat;

// View matrix inverse
uniform mat4 b3_view_mat_inv;

// Model matrix
uniform mat4 b3_model_mat;

// Normal vector transformation matrix
uniform mat3 b3_model_normal_mat;

varying vec4 color;

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
  vec3 diffuse_term = diffuse_coeff * light.color * b3_diffuse.rgb * attenuation;
  diffuse_term = clamp(diffuse_term, 0.0, 1.0);

  // Blinn-Phong model for specularities
  vec3 half_dir = normalize(surface_to_light + surface_to_eye);
  float specular_coeff = pow(max(0.0, dot(half_dir, normal)), b3_shininess);

  // Phong shading
//  vec3 reflection = normalize(-reflect(-surface_to_light, normal));
//  float specular_coeff = pow(max(0.0, dot(reflection, surface_to_eye)), 0.3 * b3_shininess);

  vec3 specular_term = specular_coeff * light.color * b3_specular.rgb * attenuation;
  specular_term = clamp(specular_term, 0.0, 1.0);

  return vec4(diffuse_term, b3_diffuse.a) +
         vec4(specular_term, b3_specular.a);
}

void main(void)
{
  vec3 normal = normalize(b3_model_normal_mat * b3_normal);
  vec3 surface_pos = vec3(b3_model_mat * b3_vert_pos);
  vec3 eye_pos = b3_view_mat_inv[2].xyz;
  vec3 surface_to_eye = normalize(eye_pos - surface_pos);

  color = vec4(0);
  for (int light_ind = 0; light_ind < B3_MAX_LIGHTS; ++light_ind) {
    color += LightContribution(b3_lights[light_ind],
        surface_pos, eye_pos, surface_to_eye, normal);
  }

  gl_Position = b3_mvp_mat * b3_vert_pos;
}

// vim: ft=glsl