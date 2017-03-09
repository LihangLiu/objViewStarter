#include "main.hpp"
#include "menus.hpp"

#ifndef GLAPIENTRY
# ifdef _WIN32
#  define GLAPIENTRY __stdcall
# else
#  define GLAPIENTRY
# endif
#endif

vec4 w_cursor;

const char* program_name = "objectViewer";

bool has_EXT_direct_state_access;

// Declared in main.hpp
ScenePtr scene;
MaterialPtr material;
LightPtr light;
float bump_height = 15.0;

vec3 eye_vector = vec3(0, 0, 10);
vec3 at_vector = vec3(0, 0, 0);
vec3 up_vector = vec3(0, 1, 0);

bool verbose = false;

bool pick_mode = false;

bool object_ctrl = false;

bool moving_eye = false;
int begin_x;
int begin_y;

bool moving_light = false;
int begin_light_x;
int begin_light_y;

float window_widthf, window_heightf;

bool wireframe = false;


void initglext()
{
    glewInit();  // Initialize OpenGL Extension Wrangler.
    if (!glewIsSupported("GL_VERSION_2_1")) {
        printf("%s: requires an OpenGL 2.1 implementation or better, exiting...\n", program_name);
        exit(1);
    }
    has_EXT_direct_state_access = !!glutExtensionSupported("GL_EXT_direct_state_access");
    //GLboolean has_compiler;
    //glGetBooleanv(GL_SHADER_COMPILER, &has_compiler);
    //if (has_compiler) printf("Has shader compiler capability.\n");
}


void initGraphics()
{
#ifdef __APPLE__
    // Seems to be needed on Apple platforms using Intel
	// integrated graphics but not elsewhere
    // If you're getting unexpected seg faults on Apple,
    // comment this out and recompile
	// If you're not seeing the mesh on a Windows box that
	// uses Intel integrated graphics, try enabling this
	// for your machine
    glEnableClientState(GL_COLOR_ARRAY);
#endif
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);  // avoid GL's dumb default of 4
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // avoid GL's dumb default of 4
    glClearColor(0.1, 0.1, 0.2, 0.0);

    scene = ScenePtr(new Scene());
    scene->setPickControl(PCtrlPtr(new PickController(scene)));
    scene->setObjControl(OCtrlPtr(new ObjectController(scene)));

    CameraPtr camera = CameraPtr(new Camera(45, 1, 0.1, 20000));

    ViewerPtr viewer = ViewerPtr(new Viewer((char*) "Cylinder", eye_vector, at_vector, up_vector, camera));
    viewer->setController(CCtrlPtr(new CylinderController(viewer)));
    scene->addViewer(viewer);
    scene->setViewer(viewer);

    viewer = ViewerPtr(new Viewer((char*) "Orbit Strafe", eye_vector, at_vector, up_vector, camera));
    viewer->setController(CCtrlPtr(new OrbStrafeController(viewer)));
    scene->addViewer(viewer);

    viewer = ViewerPtr(new Viewer((char*) "Vehicle", eye_vector, at_vector, up_vector, camera));
    viewer->setController(CCtrlPtr(new VehicleController(viewer)));
    scene->addViewer(viewer);

    envMapMenu(0);

    light = LightPtr(new Light());
    light->setCenter(vec3(0, 0, 0));
    light->setRadius(3.5);
    light->setAngleInDegrees(60.0);
    // Pick initial value from menu item
    lightMenu(1);
    scene->addLight(light);
}

/*
void printMatrices()
{
    GLfloat projection_matrix[16], modelview_matrix[16];

    glGetFloatv(GL_TRANSPOSE_PROJECTION_MATRIX, projection_matrix);
    glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, modelview_matrix);
    printf("Projection matrix:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf(" %f", projection_matrix[4 * i + j]);
        }
        printf("\n");
    }
    printf("Modelview matrix:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf(" %f", modelview_matrix[4 * i + j]);
        }
        printf("\n");
    }
}

void drawWorldCursor(vec4 p)
{
    glUseProgram(0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(value_ptr(scene->viewer->getViewMatrix()));
    glTranslatef(p.x, p.y, p.z);
    glColor3f(1,0,0);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();
}
*/

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene->draw();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    float aspect_ratio = float(w) / float(h);
    scene->viewer->camera->setAspectRatio(aspect_ratio);
    window_widthf = w;
    window_heightf = h;
}

void idle()
{
    ObjList::iterator i;
    for (i = scene->object_list.begin(); i != scene->object_list.end(); ++i) {
        (*i)->idle();
    }
}

void toggleWireframe()
{
    wireframe = !wireframe;  // toggle
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void keyboard(unsigned char key, int x, int y)
{
    if (pick_mode) {
        switch (key) {
        case 'p':
            pick_mode = !pick_mode;
            if(scene->current_object.get())
                scene->current_object->highlight(pick_mode);
            scene->pick_control->picker->reset();
        default:
            scene->pick_control->keyboard(key, x, y);
        }
        return;
    }
    switch (key) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        shaderMenu(key - '0');
        break;
    case 27:  /* Esc quits */
        exit(0);
        return;
    case 13:  /* Enter redisplays */
        break;
    case 'w':
        toggleWireframe();
        break;
    case 'V':
        verbose = !verbose;  // toggle
        break;
    case 'v':
       break;
    case 'p':
        pick_mode = !pick_mode;
        if(scene->current_object.get())
            scene->current_object->highlight(pick_mode);
        scene->pick_control->picker->reset();
        break;
    default:
        scene->viewer->controller->keyboard(key, x, y);
        scene->obj_control->keyboard(key, x, y);
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    int modifiers = glutGetModifiers();
    if (modifiers & GLUT_ACTIVE_CTRL || modifiers & GLUT_ACTIVE_SHIFT) {
        if (state == GLUT_DOWN) object_ctrl = true;
        else object_ctrl = false;
        scene->obj_control->mouse(button, state, x, y);
        return;
    }
    if (pick_mode) {
        scene->pick_control->mouse(button, state, x, y);
        // pick_mode = !pick_mode;
        return;
    }
    if (modifiers & GLUT_ACTIVE_ALT) {
        if (state == GLUT_DOWN) {
            moving_light = true;
            begin_light_x = x;
            begin_light_y = y;
        } else moving_light = false;
        return;
    }
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving_eye = true;
            scene->viewer->controller->mouse(button, state, x, y);
        } else moving_eye = false;
    }
    if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN) {
            moving_light = true;
            begin_light_x = x;
            begin_light_y = y;
        } else {
            moving_light = false;
            object_ctrl = false;
        }
    }
}

void motion(int x, int y)
{
    if (pick_mode) {
        scene->pick_control->motion(x, y);
        return;
    }
    if (moving_eye) {
        scene->viewer->controller->motion(x, y);
        return;
    }
    if (moving_light) {
        LightPtr light = scene->light_list[0];

        light->spinDegrees(0.2 * (x - begin_light_x));
        light->lift(-0.02 * (y - begin_light_y));

        glutPostRedisplay();
        begin_light_x = x;
        begin_light_y = y;
        return;
    }
    if (object_ctrl) {
        scene->obj_control->motion(x, y);
        return;
    }
}

void passivemotion(int x, int y)
{
    if (pick_mode) {
        scene->pick_control->passivemotion(x, y);
        return;
    }
    scene->viewer->controller->passivemotion(x, y);
}

void special(int key, int x, int y)
{
    if (pick_mode) {
        scene->pick_control->special(key, x, y);
        return;
    }
    scene->viewer->controller->special(key, x, y);
}

int main(int argc, char **argv)
{
#if defined(_WIN32) && !defined(NDEBUG)  // Set to 1 for Windows heap debugging
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(640, 480);
    glutInit(&argc, argv);

    glutCreateWindow(program_name);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passivemotion);
    glutSpecialFunc(special);

    initglext();
    initGraphics();
    initMenus();

    glutMainLoop();
    return 0;
}

