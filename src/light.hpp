#ifndef __light_hpp__
#define __light_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "viewer.hpp"

class Light {
private:
    vec3 color;
    vec3 center;

    float angle;
    float height;

    bool dirty;
    float radius;
    vec3 position;

    void validate();
public:
    Light();

    void setColor(const vec3 &color);
    void setCenter(vec3 center);
    void setRadius(float radius);
    void setAngleInDegrees(float degrees);
    void setAngleInRadians(float radians);

    void spinDegrees(float angle);
    void lift(float height);

    vec4 getPosition();
    vec4 getColor();

    Light& operator =(const Light& rhs);

    void draw(ViewerPtr viewer);
};
typedef shared_ptr<Light> LightPtr;

#endif // __light_hpp__
