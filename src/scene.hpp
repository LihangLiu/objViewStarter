#ifndef __scene_hpp__
#define __scene_hpp__

#include "viewer.hpp"
#include "object.hpp"

typedef vector<ObjectPtr> ObjList;

class Scene {
    public:
    ViewerPtr viewer;
    vector<ViewerPtr> viewer_list;
    vector<LightPtr> light_list;
    ObjList object_list;
    ObjectPtr current_object;
    CubeMapPtr env_map;
    OCtrlPtr obj_control;
    PCtrlPtr pick_control;

    Scene(){}
    Scene(ViewerPtr v);
    void setViewer(ViewerPtr v);
    void setObjControl(OCtrlPtr o);
    void setPickControl(PCtrlPtr p);
    void draw();
    void addObject(ObjectPtr object);
    void addLight(LightPtr light);
    void addViewer(ViewerPtr v);
    void setEnvMap(CubeMapPtr env_map);
};
typedef shared_ptr<Scene> ScenePtr;

#endif // __scene_hpp__
