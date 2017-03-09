
uniform vec4 lma; // Light-Material ambient
uniform vec4 lmd; // Light-Material diffuse
uniform vec4 lms; // Light-Material specular
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D decal;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 object_to_world;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 halfAngle;
varying vec3 c0, c1, c2;

void main()
{
  // gl_FragColor = vec4(1,0,0,1);  // XXX fix me

	lightDirection = normalize(lightDirection);
	float kd = max(0.0,lightDirection.z);
	gl_FragColor = lma + kd*lmd;
}
