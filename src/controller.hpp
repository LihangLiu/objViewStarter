#ifndef __controller_hpp__
#define __controller_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "picker.hpp"

class Viewer;
typedef shared_ptr<Viewer> ViewerPtr;
class Object;
typedef shared_ptr<Object> ObjectPtr;
enum mode_type {SPIN, MOVE};

class Controller
{
public:
    Controller();
    virtual ~Controller();
//    Controller(const Controller& other);
//    Controller& operator=(const Controller& other);

    virtual void keyboard(unsigned char key, int x, int y) = 0;
    virtual void special(int key, int x, int y) = 0;
    virtual void mouse(int button, int state, int x, int y) = 0;
    virtual void motion(int x, int y) = 0;
    virtual void passivemotion(int x, int y) = 0;
protected:
    mode_type mode;
};

typedef shared_ptr<Controller> CtrlPtr;

/******************************************************************************/

class ObjectController : public Controller
{
public:
    PickerPtr picker;
    quat lastquat;
    quat newquat;
    float bump_height = 15.0f;

    ObjectController();
    ObjectController(ScenePtr s);
    virtual ~ObjectController(){}
//    ObjectController(const ObjectController& other);
//    ObjectController& operator=(const ObjectController& other);

    void setScene(ScenePtr s);

    void keyboard(unsigned char key, int x, int y);
    void special(int key, int x, int y){}
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void passivemotion(int x, int y){}

    void stopSpinning();

protected:
    ScenePtr scene;
    ObjectPtr object;
    int begin_spin_x;
    int begin_spin_y;
    vec4 start;
    vec3 move_vector;
    mat3x3 vm;
    bool new_spin_update = true;
    bool animate_object_spinning = true;
    bool moving = false;
    bool spinning = false;

    void doTransform();
};

typedef shared_ptr<ObjectController> OCtrlPtr;

/******************************************************************************/

class PickController : public Controller
{
public:
    PickerPtr picker;

    PickController();
    PickController(ScenePtr s);
    virtual ~PickController(){};

    void setScene(ScenePtr s);

    void keyboard(unsigned char key, int x, int y);
    void special(int key, int x, int y){}
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void passivemotion(int x, int y) {}
protected:
    ScenePtr scene;
};

typedef shared_ptr<PickController> PCtrlPtr;

/******************************************************************************/

class CameraController : public Controller
{
protected:
    ViewerPtr viewer;
    int begin_x;
    int begin_y;
public:
    CameraController(ViewerPtr view);
    void setViewer(ViewerPtr view) { viewer = view; }
    void setPosition();
    virtual void keyboard(unsigned char key, int x, int y) = 0;
    virtual void special(int key, int x, int y) = 0;
    virtual void mouse(int button, int state, int x, int y) = 0;
    virtual void motion(int x, int y) = 0;
    virtual void passivemotion(int x, int y) = 0;
    virtual void reset() = 0;
    virtual void setSpeed(float s) = 0;
};

typedef shared_ptr<CameraController> CCtrlPtr;

/******************************************************************************/

class CylinderController : public CameraController
{
protected:
    float eye_angle;
    float eye_height;
    float eye_radius;
public:
    CylinderController(ViewerPtr view);
    void reset();
    void setPosition();
    void keyboard(unsigned char key, int x, int y);
    void special(int key, int x, int y){}
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void passivemotion(int x, int y){}
    void spinDegrees(float angle);
    void lift(float height);
    void setSpeed(float s){}
};

/******************************************************************************/

class OrbStrafeController : public CameraController {
protected:
    float delx;
    float dely;
    float radius;
    float view_scale;
    float step;
    vec3 upVec;
    vec3 viewVec;
    vec3 orthoVec;
    vec3 ouVec;
    vec3 lookAt;
    vec3 eye;
    bool moving;
public:
    OrbStrafeController(ViewerPtr view);
    void reset();
    void setPosition();
    void keyboard(unsigned char key, int x, int y);
    void special(int key, int x, int y);
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void passivemotion(int x, int y){}
    void setSpeed(float s);
};

/******************************************************************************/

class VehicleController : public CameraController {
protected:
    float delx;
    float dely;
    float view_scale;
    float step;
    vec3 upVec;
    vec3 viewVec;
    vec3 orthoVec;
    vec3 ouVec;
    vec3 lookAt;
    vec3 eye;
    bool moving;
public:
    VehicleController(ViewerPtr view);
    void reset();
    void setPosition();
    void keyboard(unsigned char key, int x, int y);
    void special(int key, int x, int y);
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void passivemotion(int x, int y){}
    void setSpeed(float s);
};

#endif // __controller_hpp__
