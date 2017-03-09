#ifndef __pick_hpp__
#define __pick_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "ray.hpp"

class Scene;
typedef shared_ptr<Scene> ScenePtr;
class Object;
typedef shared_ptr<Object> ObjectPtr;

typedef struct {
    float t;
    vec4 pt;
    ObjectPtr o;
} HitObj;

typedef shared_ptr<HitObj> HitPtr;
typedef std::vector<HitPtr> HitList;

extern float window_heightf, window_widthf;

/******************************************************************************/

class Picker
{
public:
    RayPtr pickRay;
    HitList hitList;
    ObjectPtr objptr;
    float cur_t;
    vec4 cur_pt;

    Picker(){}
    Picker(ScenePtr s);

    void reset();
    void setScene(ScenePtr s);
    RayPtr mouseRay(int x, int y);
    void makeList(RayPtr ray);
    bool curObj();
    bool nextObj();

protected:
    ScenePtr scene;
    HitList hitlist;
    HitList::iterator curhit;
};

typedef shared_ptr<Picker> PickerPtr;

#endif // __pick_hpp__
