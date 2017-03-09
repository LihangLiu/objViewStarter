#ifndef __global_hpp__
#define __global_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <list>
using std::list;
#include <algorithm>
using std::sort;
#include <memory>
using std::shared_ptr;

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;

typedef struct {
    vec3 n;
    float d;
} Plane;

typedef shared_ptr<Plane> PlanePtr;

typedef struct {
    vec3 lbn;
    vec3 rtf;
    vec3 centroid;
    PlanePtr planes[6];
} BBox;

void print(vec3 v);
void print(vec4 v);
void print(quat q);



#endif // __global_hpp__

