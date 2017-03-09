#include "controller.hpp"
#include "scene.hpp"
#include "ray.hpp"

#define EPSLN 0.0001

extern ScenePtr scene;
extern float window_heightf, window_widthf;

Controller::Controller() {}

Controller::~Controller() {}

// /* auxiliary */
// void print(vec3 v) {
//   printf("(%f,%f,%f)\n", v.x,v.y,v.z);
// }
// void print(vec4 v) {
//   printf("(%f,%f,%f,%f)\n", v.x,v.y,v.z,v.w);
// }


/******************************************************************************/

ObjectController::ObjectController()
{
    mode = MOVE;
    moving = false;
    picker = PickerPtr(new Picker(scene));
}

ObjectController::ObjectController(ScenePtr s) : scene(s)
{
    mode = MOVE;
    moving = false;
    picker = PickerPtr(new Picker(scene));
}

void ObjectController::setScene(ScenePtr s)
{
    scene = s;
}

void ObjectController::keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'b':
        bump_height += 1;
        printf("bump_height = %f\n", bump_height);
        // object->material->normal_map->load(bump_height);
        // object->material->normal_map->tellGL();
        // object->material->bindTextures();
        scene->current_object->material->normal_map->load(bump_height);
        scene->current_object->material->normal_map->tellGL();
        break;
    case 'B':
        bump_height -= 1;
        printf("bump_height = %f\n", bump_height);
        scene->current_object->material->normal_map->load(bump_height);
        scene->current_object->material->normal_map->tellGL();
        break;
    case 'c':
        // toggle animation
        animate_object_spinning = !animate_object_spinning;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void ObjectController::mouse(int button, int state, int x, int y)
{
    animate_object_spinning = true;

    int modifiers = glutGetModifiers();
    if (modifiers & GLUT_ACTIVE_CTRL) {
        if (button == GLUT_MIDDLE_BUTTON) {
            if (state == GLUT_DOWN) {
                /****** Get selected object and set it to spin
                      1) Use picker to return the object pointer
                      2) Set appropriate control variables in the object
                      3) Manage object highlighting as needed ******/
                printf("ctrl+middle %d %d\n",x,y);

            } else if (state == GLUT_UP && spinning) {
                /****** Set object state and highlight controls ******/
                printf("ctrl+middle up %d %d\n",x,y);
            }
            return;
        } 
        if (button == GLUT_LEFT_BUTTON) {
            /****** Enable rotation ******/
            printf("ctrl+left %d %d\n",x,y);
            if (state == GLUT_DOWN) {
                animate_object_spinning = false;
                picker->makeList(picker->mouseRay(x, y));
                if (picker->curObj()) {
                    scene->current_object = picker->objptr;
                    picker->objptr->lastquat = quat(1.0f, 0.0f, 0.0f, 0.0f);
                    lastquat = picker->objptr->curquat;
                    begin_spin_x = x;
                    begin_spin_y = y;
                }
            } else if (state == GLUT_UP) {
                if (picker->curObj()) {
                    picker->objptr->do_idle = false;
                }
            }
            return;
        }
    }
    if (modifiers & GLUT_ACTIVE_SHIFT) {
        if (button == GLUT_LEFT_BUTTON) {
            printf("shift+left %d %d\n",x,y);
            if (state == GLUT_DOWN) {
                /****** Enable spanning ******/
                picker->makeList(picker->mouseRay(x, y));
                if (picker->curObj()) {
                    scene->current_object = picker->objptr;
                    lastquat = picker->objptr->curquat;
                    begin_spin_x = x;
                    begin_spin_y = y;
                }
            } else if (state == GLUT_UP) {
                /****** Set object state and highlight controls ******/
                if (picker->curObj()) {
                    picker->objptr->do_idle = true;
                    picker->objptr->lastquat = newquat;
                }
            }
        } 
    }
    glutPostRedisplay();
}

void ObjectController::motion(int x, int y)
{
    if (spinning) {
      /****** Compute quaternion from the input
              mouse x and y and the stored old values of x and y, then
              apply it to update the rotation either here, once, or in the
              idle callback if animation mode is on ******/
    }
    if (moving) {
      /****** Compute translation, no need to animate ******/
    }
    if (animate_object_spinning) {
        if (picker->curObj()) {
            vec3 norm = vec3(0.0,0.0,1.0);
            vec3 mouseDirection = vec3(x-begin_spin_x,begin_spin_y-y,0.0);
            vec4 spinDirection = vec4(cross(norm, mouseDirection), 0.0f);
            printf("rotation\n");
            print(mouseDirection);
            print(spinDirection);

            // from object space to world
            spinDirection = inverse(scene->viewer->getViewMatrix())*spinDirection;
            // apply to current object
            vec3 a = normalize(vec3(spinDirection));
            float theta = 0.5*radians(0.5*length(mouseDirection));
            newquat = quat(cos(theta),a.x*sin(theta),a.y*sin(theta),a.z*sin(theta));
            picker->objptr->curquat = normalize(newquat*lastquat);
            // picker->objptr->lastquat = newquat;
        }

    } else {
        if (picker->curObj()) {
            vec3 norm = vec3(0.0,0.0,1.0);
            vec3 mouseDirection = vec3(x-begin_spin_x,begin_spin_y-y,0.0);
            vec4 spinDirection = vec4(cross(norm, mouseDirection), 0.0f);
            printf("rotation\n");
            print(mouseDirection);
            print(spinDirection);

            // from object space to world
            spinDirection = inverse(scene->viewer->getViewMatrix())*spinDirection;
            // apply to current object
            vec3 a = normalize(vec3(spinDirection));
            float theta = 0.5*radians(0.5*length(mouseDirection));
            picker->objptr->curquat = quat(cos(theta),a.x*sin(theta),a.y*sin(theta),a.z*sin(theta));
            picker->objptr->curquat *= lastquat;

        }
    }

    glutPostRedisplay();
}

void ObjectController::stopSpinning()
{
    object->do_idle = false;
}

void ObjectController::doTransform()
{
  /****** Do actual transformation for either rotation or translation ******/
    glutPostRedisplay();
}






/******************************************************************************/

PickController::PickController()
{
    picker = PickerPtr(new Picker());
}


PickController::PickController(ScenePtr s) : scene(s)
{
    picker = PickerPtr(new Picker(scene));
}

void PickController::setScene(ScenePtr s)
{
    scene = s;
    picker->setScene(s);
}

void PickController::keyboard(unsigned char key, int x, int y)
{
  /* Pick modes, n = pick next object pointed to, p= pick mode */
    if (key == 'n') {
        if (picker->curObj()) 
            picker->objptr->highlight(false);
        if (picker->nextObj()) {
            picker->objptr->highlight(true);
            picker->objptr->lasttrans = picker->objptr->curtrans;
            scene->current_object = picker->objptr;
        }
    }
    // if (key == 'p') {
    //     if (picker->curObj()) picker->objptr->highlight(false);
    //     picker->reset();
    // }
    glutPostRedisplay();
}

void PickController::mouse(int button, int state, int x, int y)
{
  /* Handle left button press */
    // int modifiers = glutGetModifiers();
    // if (button == GLUT_LEFT_BUTTON)
    //     if (state == GLUT_DOWN)
    //         if (picker->curObj()) {
    //             scene->current_object = picker->objptr;
    //             if (!(modifiers & GLUT_ACTIVE_CTRL))
    //                     picker->objptr->highlight(false);
    //         }

    int modifiers = glutGetModifiers();
    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {
            if (scene->current_object)
                scene->current_object->highlight(false);
            if (picker->curObj()) 
                picker->objptr->highlight(false);
            picker->makeList(picker->mouseRay(x, y));
            if (picker->curObj()) {
                picker->objptr->highlight(true);
                picker->objptr->lasttrans = picker->objptr->curtrans;
                scene->current_object = picker->objptr;
            }
        }
}

void PickController::motion(int x, int y)
{
  /* Handle motion with button press */
    
    if (picker->curObj()) {
        RayPtr new_ray = picker->mouseRay(x,y);
        vec4 new_point = new_ray->point(picker->cur_t);
        vec4 new_translate = new_point - picker->cur_pt;
        picker->objptr->curtrans = picker->objptr->lasttrans + vec3(new_translate);
    }
    glutPostRedisplay();
}


/******************************************************************************/

CameraController::CameraController(ViewerPtr view) : viewer(view) {}

void CameraController::setPosition()
{
    viewer->setPosition();
}






/******************************************************************************/

CylinderController::CylinderController(ViewerPtr view)
    : CameraController(view), eye_height(0), eye_angle(0)
{
    reset();
}

void CylinderController::reset()
{
    eye_radius = distance(viewer->eye_position, viewer->at_position);
    setPosition();
}


void CylinderController::keyboard(unsigned char c, int x, int y){}


void CylinderController::mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {
            begin_x = x;
            begin_y = y;
        }
}

void CylinderController::motion(int x, int y)
{
    spinDegrees(-0.2 * (x - begin_x));
    lift(0.02 * (y - begin_y));
    setPosition();

    glutPostRedisplay();
    begin_x = x;
    begin_y = y;
}

void CylinderController::spinDegrees(float angle)
{
    eye_angle += radians(angle);
}

void CylinderController::lift(float height)
{
    eye_height += height;
}

void CylinderController::setPosition()
{
    vec2 on_circle_position = eye_radius*vec2(sin(eye_angle), cos(eye_angle));
    viewer->eye_position = viewer->at_position
        + vec3(on_circle_position.x, eye_height, on_circle_position.y);
    viewer->setPosition();
}





/******************************************************************************/

OrbStrafeController::OrbStrafeController(ViewerPtr view) : CameraController(view)
{
    reset();
}

void OrbStrafeController::reset()
{
  /******Initialize camera state******/
    delx = 0;
    dely = 0;
    radius = distance(viewer->eye_position, viewer->at_position);
    setPosition();
}

void OrbStrafeController::keyboard(unsigned char key, int x, int y)
{
  /******Handle character key controls here ******/
    switch (key) {
    case 'a':
        delx += 20;
        break;
    case 'd':
        delx -= 20;
        break;
    case 'w':
        // been occupied
        break;
    case 's':
        dely += 10;
        break;
    default:
        break;
    }
    setPosition();
    glutPostRedisplay();

}

void OrbStrafeController::special(int key, int x, int y)
{
  /******Handle special key controls here (e.g. arrow keys)******/
    // printf("%d\n", key);
    switch (key) {
    case 100:
      delx += 20;
      break;
    case 102:
      delx -= 20;
      break;
    case 101:
      dely -= 10;
      break;
    case 103:
      dely += 10;
      break;
    default:
      break;
    }
    setPosition();
    glutPostRedisplay();
}

void OrbStrafeController::mouse(int button, int state, int x, int y)
{
  /******Handle mouse inputs here******/
    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {
            begin_x = x;
            begin_y = y;
        }
}

void OrbStrafeController::motion(int x, int y)
{
    /* Orbital camera w/o using spherical coordinates */
    if (moving) {
     /******Move the camera******/
    }

    delx += (x-begin_x);
    dely += (y-begin_y);
    
    setPosition();

    glutPostRedisplay();
    begin_x = x;
    begin_y = y;
}

void OrbStrafeController::setPosition()
{
     /******Set the camera position (may not be the same as Cylinder)******/
    float theta = radians(-0.05*delx);
    if ((0.1*dely+radius)/radius<0.01) {
        dely = (0.01-1)*radius/0.1;
    }
    view_scale = (0.1*dely+radius)/radius;
    // printf("%f\n", view_scale);

    vec3 on_circle_position = view_scale*radius*vec3(sin(theta), 0.0, cos(theta));
    viewer->eye_position = viewer->at_position + on_circle_position;
    viewer->setPosition();
}

void OrbStrafeController::setSpeed(float s)
{
     /******Control the speed******/
}






/******************************************************************************/

VehicleController::VehicleController(ViewerPtr view) : CameraController(view)
{
    reset();
}

void VehicleController::reset()
{
  /******Initialize camera state******/
    eye = viewer->eye_position;
    viewVec = normalize(viewer->at_position-eye);
    setPosition();
}

void VehicleController::keyboard(unsigned char key, int x, int y)
{
  /******Handle character key controls here ******/
    switch (key) {
    case 'a':
        delx = 20;
        break;
    case 'd':
        delx = -20;
        break;
    case 'w':
        // been occupied
        break;
    case 's':
        eye -= viewVec;
        break;
    default:
        break;
    }
    setPosition();
    glutPostRedisplay();
}

void VehicleController::special(int key, int x, int y)
{
  /******Handle special key controls here (e.g. arrow keys)******/
    switch (key) {
    case 100:
      delx = 20;
      break;
    case 102:
      delx = -20;
      break;
    case 101:
      eye += viewVec;
      break;
    case 103:
      eye -= viewVec;
      break;
    default:
      break;
    }
    setPosition();
    glutPostRedisplay();
}

void VehicleController::mouse(int button, int state, int x, int y)
{
  /******Handle mouse inputs here******/
    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {
            begin_x = x;
            begin_y = y;
        }
}

void VehicleController::motion(int x, int y)
{
    if (moving) {
     /******Move the camera******/
    }

    delx = (x-begin_x);
    dely = (y-begin_y);
    
    setPosition();

    glutPostRedisplay();
    begin_x = x;
    begin_y = y;
}

mat3 rorateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(  c,0.0, -s,
                0.0,1.0,0.0,
                  s,0.0,  c);
}

mat3 rorateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(1.0,0.0,0.0,
                0.0,  c,  s,
                0.0, -s,  c);
}

void VehicleController::setPosition()
{
     /******Set the camera position (may not be the same as Cylinder)******/
    float theta_x = radians(0.1*delx);
    mat3 ry = rorateY(theta_x);
    float theta_y = radians(0.1*dely);
    mat3 rx = rorateX(theta_y);
    viewVec = ry*rx*viewVec;


    viewer->eye_position = eye;
    viewer->at_position = eye + viewVec;
    viewer->setPosition();

    delx = 0;
    dely = 0;
}

void VehicleController::setSpeed(float s)
{
     /******Control the speed******/
}
