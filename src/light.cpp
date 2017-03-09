#include "light.hpp"

Light::Light()
    : color(vec3(1))
    , center(vec3(0))
    , radius(1)
    , height(0)
    , dirty(true)
    , position(vec3(0,0,1))
{}

void Light::setColor(const vec3 &color_)
{
    color = color_;
}

void Light::setCenter(vec3 center_)
{
    center = center_;
    dirty = true;
}

void Light::setRadius(float radius_)
{
    radius = radius_;
    dirty = true;
}

void Light::setAngleInDegrees(float angle_)
{
    angle = radians(angle_);
    dirty = true;
}

void Light::setAngleInRadians(float angle_)
{
    angle = angle_;
    dirty = true;
}

void Light::spinDegrees(float angle_)
{
    angle += radians(angle_);
    dirty = true;
}

void Light::lift(float height_)
{
    height += height_;
    dirty = true;
}

void Light::validate()
{
    if (dirty) {
        vec2 on_circle_position = radius*vec2(sin(angle), cos(angle));
        position = center + vec3(on_circle_position.x, height, on_circle_position.y);
#if 0
        OUTPUT(position);
#endif
        dirty = false;
    }
}

Light& Light::operator =(const Light& rhs)
{
    if (this != &rhs) {
        color = rhs.color;
        position = rhs.position;
        center = rhs.center;

        angle = rhs.angle;
        height = rhs.height;

        dirty = rhs.dirty;
    }
    return *this;
}

vec4 Light::getPosition()
{
    validate();
    return vec4(position, 1);
}

vec4 Light::getColor()
{
    validate();
    return vec4(color, 1);
}

static void glColor(vec3 color)
{
    glColor3fv(reinterpret_cast<const GLfloat*>(&color));
}

void Light::draw(ViewerPtr viewer)
{
    validate();

    glUseProgram(0);  // fixed-function
    glColor(color);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();
}
