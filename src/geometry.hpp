#ifndef __geometry_h__
#define __geometry_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "global.hpp"
#include "tiny_obj_loader.h"

/******************************************************************************/

class Geometry {
public:
    BBox aabb;

    virtual void draw() = 0;
    virtual void makeAabb() = 0;
};
typedef shared_ptr<Geometry> GeometryPtr;

/******************************************************************************/

class Mesh2D : public Geometry {
private:
    vec2 xy_min, xy_max;
    ivec2 steps;
    GLuint vbo; // OpenGL Vertex Buffer Object (VBO)
    bool valid;

    // glDrawElements indices
    size_t num_ndxs;
    GLuint* ndx;

    void validate();

public:
    Mesh2D(vec2 xy_min, vec2 xy_max, ivec2 steps);
    ~Mesh2D();

    void draw();
    void makeAabb();
};
typedef shared_ptr<Mesh2D> Mesh2DPtr;

/******************************************************************************/

typedef struct {
    vec3 xyz;
    vec3 n;
    vec2 st;
} Vtx;

class ObjMesh : public Geometry
{
public:
    int index;
    const char* name;

    ObjMesh(tinyobj::shape_t s);
    ~ObjMesh();

    void draw();
    void makeAabb();
private:
    GLuint vao; // OpenGL Vertex Array Object (VAO)
    GLuint vbo; // OpenGL Vertex Buffer Object (VBO)
    GLuint ebo; //OpenGL Element (index) Buffer

    bool valid;

    size_t num_ndxs; // Number of indices

    tinyobj::shape_t shape;

    void setUpVBO();
};
typedef shared_ptr<ObjMesh> ObjMeshPtr;

#endif // __geometry_h__
