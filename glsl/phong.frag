uniform vec4 lma; // Light-Material ambient
uniform vec4 lmd; // Light-Material diffuse
uniform vec4 lms; // Light-Material specular
uniform float gloss;

uniform bool decal_flag;
uniform bool amb_flag;
uniform bool diff_flag;
uniform bool spec_flag;
uniform bool gloss_flag;
uniform bool bump_flag;
uniform bool env_flag;

uniform sampler2D ambient_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;
uniform sampler2D gloss_map;
uniform sampler2D normal_map;

varying vec3 e_normal;
varying vec3 light_direction;
varying vec3 half_angle;
varying vec2 tex_coords;

void main()
{
  vec4 acol = lma;
  vec4 dcol = lmd;
  vec4 scol = lms;
  float sh = gloss;
  if (amb_flag) acol *= texture2D(ambient_map, tex_coords);
  if (diff_flag) dcol *= texture2D(diffuse_map, tex_coords);
  if (spec_flag) scol *= texture2D(specular_map, tex_coords);
  //if (gloss_flag) sh *= texture2D(gloss_map, tex_coords);
  vec3 norm_light_dir = normalize(light_direction);
  vec3 norm_halfangle = normalize(half_angle);
  float diffuse = clamp((dot(e_normal, norm_light_dir)), 0.0, 1.0);
  float specular = clamp((dot(e_normal, norm_halfangle)), 0.0, 1.0);
  if (diffuse <= 0.0) specular = 0.0;
  else specular = pow(specular, sh);
  gl_FragColor = (dcol * diffuse + acol) + scol * specular;
}

