#include "ray.hpp"


/* auxiliary */
void print(vec3 v) {
  printf("(%f,%f,%f)\n", v.x,v.y,v.z);
}
void print(vec4 v) {
  printf("(%f,%f,%f,%f)\n", v.x,v.y,v.z,v.w);
}

void print(quat q) {
  printf("(%f,%f,%f,%f)\n", q.w,q.x,q.y,q.z);
}


Ray::Ray(vec4 o, vec4 d) : origin(o), direction(d)
{
    direction = normalize(direction);
}

Ray& Ray::operator =(const Ray& rhs)
{
    if (this != &rhs) {
        origin = rhs.origin;
        direction = rhs.direction;
    }
    return *this;
}

void Ray::transform(mat4x4 m)
{
  /****** Tranform the ray (origin and direction vector) ******/
    vec4 pixel_at_world = m*(origin+direction);
    origin = m*origin;
    direction = normalize(pixel_at_world - origin);
}

// ray has transform to object space
float Ray::intersect(BBox aabb)
{
  /****** Return t of intersection with given bounding box ******/
    if (ifIntersect(aabb)) {
        vec3 link = aabb.centroid - vec3(origin.x,origin.y,origin.z);
        return length(link);
    }
    return 0.0f;
}

bool Ray::ifIntersect(BBox aabb)
{
  /****** Return t of intersection with given bounding box ******/
    vec3 R0 = vec3(origin.x,origin.y,origin.z);
    vec3 Rd = vec3(direction.x,direction.y,direction.z);
    vec3 bmin = aabb.lbn;
    vec3 bmax = aabb.rtf;
    float tMin = -1.0e30; // 1.0e308 is close to infinity... close enough for us!
    float tMax = 1.0e30;
    double ttemp;

    for (int currentaxis = 0; currentaxis < 3; currentaxis++) {
      double vd = Rd[currentaxis];
      // if the ray is parallel to the face's plane (=0.0)
      if( vd == 0.0 ) continue;
      double v1 = bmin[currentaxis] - R0[currentaxis];
      double v2 = bmax[currentaxis] - R0[currentaxis];
      // two slab intersections
      double t1 = v1/vd;
      double t2 = v2/vd;
      if ( t1 > t2 ) { // swap t1 & t2
        ttemp = t1;
        t1 = t2;
        t2 = ttemp;
      }
      if (t1 > tMin) tMin = t1;
      if (t2 < tMax) tMax = t2;
      if (tMin > tMax) return false; // box is missed
      if (tMax < 0.0f) return false; // box is behind ray
    }
     return true; 
}

vec4 Ray::point(float t)
{
  /****** Return point at t along ray ******/
    vec4 p = origin + t*direction;
    return p;
}

void Ray::cout() {
    printf("origin:");
    print(origin);
    printf("direction:");
    print(direction);
}