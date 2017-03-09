#ifndef __object_hpp__
#define __object_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "glslshader.hpp"
#include "glslprogram.hpp"
#include "geometry.hpp"
#include "light.hpp"
#include "material.hpp"
#include "controller.hpp"

/******************************************************************************/

class Object
{
public:
    string vertex_filename;
    string fragment_filename;
    GLSLProgram program;

    BBox aabb;
    mat4x4 baseMat;
    mat4x4 transform;
    MaterialPtr material;
    quat curquat, lastquat;
    vec3 curtrans, lasttrans;
    bool do_idle;

    Object()
    {
        highlighted = false;
        do_idle = false;
        baseMat = mat4x4(1);
    }

    Object(MaterialPtr m) : material(m)
    {
        highlighted = false;
        do_idle = false;
        baseMat = mat4x4(1);
    }

    virtual void draw(ViewerPtr viewer, LightPtr light) = 0;
    virtual void idle() = 0;
    virtual void loadProgram() = 0;
    virtual void makeAabb() = 0;
    void drawAabb();
    void setMaterial(MaterialPtr m) { material = m; }
    void highlight(bool h);

protected:
    bool highlighted;
    void makePlanes();
    PlanePtr makePlane(vec3 p0, vec3 p1, vec3 p2);
};

/******************************************************************************/

class Torus : public Object
{
public:
    float outerRad;
    float innerRad;

    Torus(float i, float o);
    Torus(float i, float o, MaterialPtr m);
    ~Torus();

    void setMaterial(MaterialPtr m);
    void loadProgram();
    void draw(ViewerPtr viewer, LightPtr light);
    void idle();
    void makeAabb();

private:
    Mesh2DPtr mesh2d;

    void init();
};
typedef shared_ptr<Torus> TorusPtr;

/******************************************************************************/

class ObjFileObject : public Object
{
public:
    ObjFileObject();
    //~ObjFileObject();

    void setMaterial(MaterialPtr m){}
    void loadProgram();
    bool loadObjFile();
    void draw(ViewerPtr viewer, LightPtr light);
    void idle();
    void makeAabb();

    const char* obj_filename;
    const char* obj_basepath;
private:
    vector<MaterialPtr> materials;
    vector<ObjMeshPtr> meshes;

    void init();
    void makeMaterial(tinyobj::material_t m);
    void makeMesh(tinyobj::shape_t s);
};
typedef shared_ptr<ObjFileObject> ObjFileObjectPtr;


#endif // __object_hpp__
