#include "scene.hpp"

#define OUTPUT(x) std::cout << #x " = " << x << std::endl;

extern bool verbose;
extern float window_widthf, window_heightf;

/******************************************************************************/

Scene::Scene(ViewerPtr v) : viewer(v) {}

void Scene::setViewer(ViewerPtr v)
{
    viewer = v;
}

void Scene::draw()
{
    for (size_t i=0; i<object_list.size(); i++) {
        object_list[i]->draw(viewer, light_list[0]);
    }
    for (size_t i=0; i<light_list.size(); i++) {
        LightPtr light = light_list[i];

        light->draw(viewer);
    }
    if (env_map) {
        env_map->draw(10000);
    }
}

void Scene::setObjControl(OCtrlPtr o)
{
    obj_control = o;
}

void Scene::setPickControl(PCtrlPtr p)
{
    pick_control = p;
}

void Scene::addObject(ObjectPtr object)
{
    object_list.push_back(object);
}

void Scene::addLight(LightPtr light)
{
    light_list.push_back(light);
}

void Scene::addViewer(ViewerPtr v)
{
  viewer_list.push_back(v);
}

void Scene::setEnvMap(CubeMapPtr cm)
{
    env_map = cm;
}
