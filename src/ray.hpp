#ifndef __ray_hpp__
#define __ray_hpp__

#include "global.hpp"

class Ray
{
public:
    vec4 origin;
    vec4 direction;

    Ray(){}
    Ray(vec4 o, vec4 d);
    Ray& operator =(const Ray& rhs);

    void transform(mat4x4 m);
    float intersect(BBox);
    vec4 point(float t);

    void cout();

private:
	bool ifIntersect(BBox);
};

typedef shared_ptr<Ray> RayPtr;

#endif // __ray_hpp__
