#ifndef __camera_hpp__
#define __camera_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "global.hpp"

class CameraController;
typedef shared_ptr<CameraController> CCtrlPtr;

/******************************************************************************/

class Camera
{
    float fov_degrees;
    float aspect_ratio;

    mat4x4 projection_matrix;

public:
    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;

    Camera(float fov, float ar, float zn, float zf);
    Camera(const Camera& other);

    Camera& operator =(const Camera& rhs);

    void setAspectRatio(float ar);

    void setGlProjection();

    void setProjection();
protected:
    void reset();
};

typedef shared_ptr<Camera> CameraPtr;

/******************************************************************************/

class Viewer
{
protected:
    vec3 initial_at_position;
    vec3 initial_eye_position;
    mat4x4 view_matrix;

public:
    char* name;
    vec3 up_vector;
    vec3 at_position;
    vec3 eye_position;
    CameraPtr camera;
    CCtrlPtr controller;

    Viewer(char* name, vec3 eye, vec3 lookat, vec3 up, CameraPtr cam);
    Viewer& operator =(const Viewer& rhs);

    void reset(vec3 eye, vec3 lookat);
    void setCamera(CameraPtr cam);
    void setController(CCtrlPtr c);

    void setPosition();

    mat4x4 getViewMatrix();

    void setGlView();
};

typedef shared_ptr<Viewer> ViewerPtr;

#endif // __camera_hpp__
