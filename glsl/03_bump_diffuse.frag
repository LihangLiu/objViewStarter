
uniform vec4 lma; // Light-Material ambient
uniform vec4 lmd; // Light-Material diffuse
uniform vec4 lms; // Light-Material specular
uniform float shininess;

uniform sampler2D normal_map;
uniform sampler2D decal;
uniform sampler2D heightField;
uniform samplerCube env_map;

uniform mat3 object_to_world;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 halfAngle;
varying vec3 c0, c1, c2;

void main()
{
  // gl_FragColor = vec4(1,0,0,1);  // XXX fix me

	vec4 perturbedNormal = texture(normal_map, normalMapTexCoord);
	lightDirection = normalize(lightDirection);
	float kd = dot(lightDirection,vec3(perturbedNormal.x, perturbedNormal.y, perturbedNormal.z));
	kd = max(0.0,kd);
	gl_FragColor = lma + kd*lmd;
}