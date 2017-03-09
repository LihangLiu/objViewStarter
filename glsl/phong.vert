attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texCoords;

uniform vec3 light_position;

varying vec3 e_normal;
varying vec3 light_direction;
varying vec3 half_angle;
varying vec2 tex_coords;

void main()
{
  tex_coords = texCoords;
  gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex, 1.0);
  e_normal = gl_NormalMatrix * normal;
  vec3 e_pos = vec3(gl_ModelViewMatrix * vec4(vertex, 1.0));
  vec3 eye_direction = -e_pos;
  light_direction = light_position - e_pos;
  half_angle = normalize(normalize(light_direction) + normalize(-e_pos));
}

