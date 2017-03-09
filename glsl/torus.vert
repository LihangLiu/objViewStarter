
attribute vec2 parametric;

uniform vec3 light_position;  // Object-space
uniform vec3 eye_position;    // Object-space
uniform vec2 torus_info;

varying vec2 normalMapTexCoord;

varying vec3 lightDirection;
varying vec3 halfAngle;
varying vec3 eyeDirection;
varying vec3 c0, c1, c2;

float PI = 3.14159;

void main()
{
  // normalMapTexCoord = vec2(0);  // XXX fix me
  // gl_Position = gl_ModelViewProjectionMatrix * vec4(parametric, 0, 1);  // XXX fix me
  // eyeDirection = vec3(0);  // XXX fix me
  // lightDirection = vec3(0);  // XXX fix me
  // halfAngle = vec3(0);  // XXX fix me
  // c0 = vec3(0);  // XXX fix me
  // c1 = vec3(0);  // XXX fix me
  // c2 = vec3(0);  // XXX fix 

	// rool up square to torus
	float u = parametric.x;
	float v = parametric.y;
	float theta = 2.0*PI*v;
	float phi = 2.0*PI*u;
	float R = torus_info.x;
	float r = torus_info.y;
	float x = (R+r*cos(theta))*cos(phi);
	float y = (R+r*cos(theta))*sin(phi);
	float z = r*sin(theta);
	gl_Position = gl_ModelViewProjectionMatrix * vec4(x,y,z, 1);

	// set texture coord
	normalMapTexCoord = vec2(mod(u*6.0,1.0),mod(v*2.0,1.0));

	// diffuse
	vec3 Tu = vec3(	(R+r*cos(theta))*(-2.0*PI*sin(phi)),
					(R+r*cos(theta))*(2.0*PI*cos(phi)),
					0.0);
	Tu = normalize(Tu);
	vec3 Tv = vec3(	r*cos(phi)*(-2.0*PI*sin(theta)),
					r*sin(phi)*(-2.0*PI*sin(theta)),
					r*2.0*PI*cos(theta));
	Tv = normalize(Tv);
	vec3 N = cross(Tu,Tv);
	N = normalize(N);
	vec3 B = cross(Tu,N);
	mat3 M = mat3(Tu,B,N);
	
	vec3 vert2light = light_position - vec3(x,y,z);
	lightDirection = transpose(M)*vert2light;

	// specular
	vec3 vert2eye = eye_position - vec3(x,y,z);
	eyeDirection = 	transpose(M)*vert2eye;
	halfAngle = (lightDirection+eyeDirection)/2.0;

	// keep M
	c0 = Tu;
	c1 = B;
	c2 = N;

}

