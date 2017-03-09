#include "picker.hpp"
#include "scene.hpp"



/******************************************************************************/

Picker::Picker(ScenePtr s) : scene(s)
{
    reset();
}

void Picker::reset()
{
  /****** Initialize (or reset to initial state) ******/
    cur_t = 0.0f;
    cur_pt = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    objptr = 0;
    hitList.clear();
}

void Picker::setScene(ScenePtr s)
{
    scene = s;
    reset();
}

RayPtr Picker::mouseRay(int x, int y)
{
  /****** Make a world coordinate ray from mouse x and y coords ******/
    // RayPtr curRay = RayPtr(new Ray(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f)));

    ViewerPtr viewer = scene->viewer;
    CameraPtr camera = viewer->camera;
    float l = camera->left;
    float r = camera->right;
    float b = camera->bottom;
    float t = camera->top;
    float near = camera->near;
    vec4 pixel_at_screen = vec4(l+x/window_widthf*(r-l), t-y/window_heightf*(t-b), -near, 0.0f);
    RayPtr curRay = RayPtr(new Ray(vec4(0.0f, 0.0f, 0.0f, 1.0f), pixel_at_screen));
    curRay->transform(inverse(viewer->getViewMatrix()));
    printf("ray\n");
    print(curRay->origin);
    print(curRay->direction);

    return curRay;
}

// get the first
void Picker::makeList(RayPtr ray)
{
  /****** Make a list of objects hit by the ray ******/
    reset();
    pickRay = ray;

    ObjList::iterator i;
    for (i = scene->object_list.begin(); i != scene->object_list.end(); ++i) {
        // transform ray to object space, calculate intersection and transform back
        ObjectPtr cur_obj= (*i);
        cur_obj->do_idle = true;
        RayPtr new_ray = RayPtr(new Ray(ray->origin, ray->direction));

        // to object space
        // new_ray->cout();
        new_ray->transform(inverse(cur_obj->transform));
        // new_ray->cout();
        cur_t = new_ray->intersect(cur_obj->aabb);
        cur_pt = new_ray->point(cur_t);

        // back to world space
        cur_pt = cur_obj->transform*cur_pt;
        cur_t = length(cur_pt-ray->origin);

        if (cur_t > scene->viewer->camera->near) {
            printf("intersect %f\n", cur_t);
            print(cur_pt);
            // objptr = cur_obj;
            HitObj hitobj = {cur_t,cur_pt,cur_obj};
            HitPtr hitPtr = HitPtr(new HitObj(hitobj));
            hitList.push_back(hitPtr);
        }
    }
    printf("hit list %d\n", hitList.size());
}

bool Picker::nextObj()
{
  /****** Get the next ojbect in the list ******/
    if (!hitList.empty()) 
        hitList.erase(hitList.begin());
    return curObj();
}


bool Picker::curObj()
{
  /****** Check to see if there's a current object ******/
    if (!hitList.empty()) {
        HitPtr hitPtr = hitList.front();
        cur_t = hitPtr->t;
        cur_pt = hitPtr->pt;
        objptr = hitPtr->o;
        return true;
    }
    return false;
}
