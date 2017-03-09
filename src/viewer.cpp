#include "viewer.hpp"
#include "controller.hpp"
#include "menus.hpp"

extern float window_heightf, window_widthf;

/******************************************************************************/

Camera::Camera(float fov, float ar, float zn, float zf)
    : fov_degrees(fov)
    , aspect_ratio(ar)
    , near(zn)
    , far(zf)
{
    reset();
}

void Camera::reset()
{
    top = tan(radians(fov_degrees / 2.0)) * near;
    bottom = -top;
    right = top * aspect_ratio;
    left = -right;
    setProjection();
}

Camera::Camera(const Camera& other)
{
    fov_degrees = other.fov_degrees;
    aspect_ratio = other.aspect_ratio;
    left = other.left;
    right = other.right;
    bottom = other.bottom;
    top = other.top;
    near = other.near;
    far = other.far;
    projection_matrix = other.projection_matrix;
}

Camera& Camera::operator =(const Camera& rhs)
{
    if (this != &rhs) {
        fov_degrees = rhs.fov_degrees;
        aspect_ratio = rhs.aspect_ratio;
        left = rhs.left;
        right = rhs.right;
        bottom = rhs.bottom;
        top = rhs.top;
        near = rhs.near;
        far = rhs.far;
        projection_matrix = rhs.projection_matrix;
    }
    return *this;
}

void Camera::setAspectRatio(float ar)
{
    aspect_ratio = ar;
    reset();
}

void Camera::setGlProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(value_ptr(projection_matrix));
}

void Camera::setProjection()
{
    projection_matrix = perspective(radians(fov_degrees), aspect_ratio, near, far);
    setGlProjection();
}

/******************************************************************************/

Viewer::Viewer(char* n, vec3 eye, vec3 lookat, vec3 up, CameraPtr cam)
    : name(n)
    , initial_eye_position(eye)
    , initial_at_position(lookat)
    , up_vector(up)
    , camera(cam)
    , controller(0)
{
    reset(eye, lookat);
}

Viewer& Viewer::operator =(const Viewer& rhs)
{
    if (this != &rhs) {
        initial_at_position = rhs.initial_at_position;
        initial_eye_position = rhs.initial_eye_position;

        up_vector = rhs.up_vector;
        at_position = rhs.at_position;
        eye_position = rhs.eye_position;

        camera = rhs.camera;

        view_matrix = rhs.view_matrix;
    }
    return *this;
}

void Viewer::reset(vec3 eye, vec3 lookat)
{
    eye_position = eye;
    at_position = lookat;
    setPosition();
    if (controller.get()) controller->reset();
}

void Viewer::setCamera(CameraPtr c)
{
    camera = c;
    camera->setAspectRatio(window_widthf/window_heightf);
}

void Viewer::setController(CCtrlPtr c)
{
    controller = c;
}

void Viewer::setPosition()
{
    view_matrix = lookAt(eye_position, at_position, up_vector);
    setGlView();
}

mat4x4 Viewer::getViewMatrix()
{
    setPosition();
    return view_matrix;
}

void Viewer::setGlView()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(value_ptr(view_matrix));
}

