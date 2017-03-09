#include "main.hpp"
#include "menus.hpp"
#include "object.hpp"
#include "material.hpp"
#include <glob.h>
#include <string>


extern vec3 eye_vector;
extern vec3 at_vector;

/******************************************************************************/

static const struct {
    const char *name;
    const char *filename_pattern;
} envmap_list[] = {
    /*
    { "Cloudy hills", "images/cloudyhills_%s.tga" },
    { "Foggy desert", "images/foggydesert_%s.tga" },
    { "In clouds", "images/inclouds_%s.tga" },
    { "Nighttime", "images/night_%s.tga" },
    { "Tron world", "images/tron_%s.tga" },
    { "Lava world", "images/lava_%s.tga" },
    */
    { "Rugged Terrain", "images/terrain_%s.png" },
    { "Barren Terrain", "images/autumn_%s.png" },
    { "Wilderness", "images/brudslojan_%s.png" },
    { "Ice River", "images/iceriver_%s.png" },
    { "Forbidden City", "images/beijing_%s.png" },
    { "Museum", "images/unwrapped_%s.png" },
    { "NVidia Lobby", "images/nvlobby_%s.png" },
    { "Earth", "images/earth_%s.png" },
    { "Escher", "images/escher_%s.png" },
    { "Alps", "images/alps_%s.tga" },
    { "Hexagon", "images/hexagon_%s.tga" },
    { "Stone Gods", "images/stonegods_%s.tga" },
};

void envMapMenu(int item)
{
    assert(item < (int)countof(envmap_list));
    CubeMapPtr env_map(new CubeMap(envmap_list[item].filename_pattern));
    env_map->load();
    env_map->tellGL();
    scene->setEnvMap(env_map);
    for (int i = 0; i < scene->object_list.size(); i++)
        scene->object_list[i]->material->env_map = env_map;
    glutPostRedisplay();
}

/******************************************************************************/

static const struct {
    const char *name;
    const char *filename;
} bumpy_list[] = {
    { "Outward bumps", "images/bumps_out.tga" },
    { "Inward bumps", "images/bumps_in.tga" },
    { "Brick", "images/brick.tga" },
    { "GeForce cell", "images/geforce_cell.tga" },
    { "GeForce etch", "images/geforce_etch.tga" },
    { "Mosaic", "images/mosaic.tga" },
    { "Stripes", "images/stripes.tga" },
    { "Texas Longhorn", "images/texas_longhorn.tga" },
    { "Texas Longhorn 2", "images/texas_longhorn2.tga" },
};

void bumpyMenu(int item)
{
    assert(item < (int)countof(bumpy_list));

    const char *filename = bumpy_list[item].filename;

    NormalMapPtr normal_map = NormalMapPtr(new NormalMap(filename));
    float bump_height = scene->obj_control->bump_height;
    normal_map->load(bump_height);
    normal_map->tellGL();
    Texture2DPtr height_field(new Texture2D(filename));
    height_field->load();
    height_field->tellGL();
    scene->current_object->material->normal_map = normal_map;
    glutPostRedisplay();
}

/******************************************************************************/

static const struct {
    const char *name;
    const char *filename;
} decal_list[] = {
    { "Texas Longhorn", "images/texas_longhorn_decal.tga" },
    { "Solid white", "images/solid_white.tga" },
    { "Brick", "images/brick_decal.tga" },
    { "GeForce", "images/geforce.tga" },
    { "Solid green", "images/solid_green.tga" },
    { "Green stripes", "images/green_stripes.tga" },
    { "JPG Test", "objects/Columns1.jpg" }
};

void decalMenu(int item)
{
    assert(item < (int)countof(decal_list));
    const char *filename = decal_list[item].filename;
    Texture2DPtr decal(new Texture2D(filename));
    decal->load();
    decal->tellGL();
    scene->current_object->material->decal = decal;
    glutPostRedisplay();
}

/******************************************************************************/

static const struct {
    const char *name;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float gloss;
} material_list[] = {
    { "Matte white", vec3(0.1, 0.1, 0.1), vec3(0.9, 0.9, 0.9), vec3(0), 0 },
    { "Emerald", vec3(0.0215, 0.1745, 0.0215), vec3(0.07568, 0.61424, 0.07568), vec3(0.633, 0.727811, 0.633), 0.6*128 },
    { "Jade", vec3(0.135, 0.2225, 0.1575), vec3(0.54, 0.89, 0.63), vec3(0.316228, 0.316228, 0.316228), 0.1*128 },
    { "Obsidian", vec3(0.05375, 0.05, 0.06625), vec3(0.18275, 0.17, 0.22525), vec3(0.332741, 0.328634, 0.346435), 0.3*128 },
    { "Pearl", vec3(0.25, 0.20725, 0.20725), vec3(1, 0.829, 0.829), vec3(0.296648, 0.296648, 0.296648), 0.088*128 },
    { "Ruby", vec3(0.1745, 0.01175, 0.01175), vec3(0.61424, 0.04136, 0.04136), vec3(0.727811, 0.626959, 0.626959), 0.6*128 },
    { "Turquoise", vec3(0.1, 0.18725, 0.1745), vec3(0.396, 0.74151, 0.69102), vec3(0.297254, 0.30829, 0.306678), 0.1*128 },
    { "Brass", vec3(0.329412, 0.223529, 0.027451), vec3(0.780392, 0.568627, 0.113725), vec3(0.992157, 0.941176, 0.807843), 0.21794872*128 },
    { "Bronze", vec3(0.2125, 0.1275, 0.054), vec3(0.714, 0.4284, 0.18144), vec3(0.393548, 0.271906, 0.166721), 0.2*128 },
    { "Chrome", vec3(0.25, 0.25, 0.25), vec3(0.4, 0.4, 0.4), vec3(0.774597, 0.774597, 0.774597), 0.6*128 },
    { "Copper", vec3(0.19125, 0.0735, 0.0225), vec3(0.7038, 0.27048, 0.0828), vec3(0.256777, 0.137622, 0.086014), 0.1*128 },
    { "Gold", vec3(0.24725, 0.1995, 0.0745), vec3(0.75164, 0.60648, 0.22648), vec3(0.628281, 0.555802, 0.366065), 0.4*128 },
    { "Silver", vec3(0.19225, 0.19225, 0.19225), vec3(0.50754, 0.50754, 0.50754), vec3(0.508273, 0.508273, 0.508273), 0.4*128 },
    { "Black plastic", vec3(0.0, 0.0, 0.0), vec3(0.01, 0.01, 0.01), vec3(0.50, 0.50, 0.50), .25*128 },
    { "Cyan plastic", vec3(0.0, 0.1, 0.06), vec3(0.0, 0.50980392, 0.50980392), vec3(0.50196078, 0.50196078, 0.50196078), .25*128 },
    { "Green plastic", vec3(0.0, 0.0, 0.0), vec3(0.1, 0.35, 0.1), vec3(0.45, 0.55, 0.45), .25*128 },
    { "Red plastic", vec3(0.0, 0.0, 0.0), vec3(0.5, 0.0, 0.0), vec3(0.7, 0.6, 0.6), .25*128 },
    { "White plastic", vec3(0.0, 0.0, 0.0), vec3(0.55, 0.55, 0.55), vec3(0.70, 0.70, 0.70), .25*128 },
    { "Yellow plastic", vec3(0.0, 0.0, 0.0), vec3(0.5, 0.5, 0.0), vec3(0.60, 0.60, 0.50), .25*128 },
    { "Black rubber", vec3(0.02, 0.02, 0.02), vec3(0.01, 0.01, 0.01), vec3(0.4, 0.4, 0.4), .078125*128 },
    { "Cyan rubber", vec3(0.0, 0.05, 0.05), vec3(0.4, 0.5, 0.5), vec3(0.04, 0.7, 0.7), .078125*128 },
    { "Green rubber", vec3(0.0, 0.05, 0.0), vec3(0.4, 0.5, 0.4), vec3(0.04, 0.7, 0.04), .078125*128 },
    { "Red rubber", vec3(0.05, 0.0, 0.0), vec3(0.5, 0.4, 0.4), vec3(0.7, 0.04, 0.04), .078125*128 },
    { "White rubber", vec3(0.05, 0.05, 0.05), vec3(0.5, 0.5, 0.5), vec3(0.7, 0.7, 0.7), .078125*128 },
    { "Yellow rubber", vec3(0.05, 0.05, 0.0), vec3(0.5, 0.5, 0.4), vec3(0.7, 0.7, 0.04), .078125*128 }
};

void materialMenu(int item)
{
    assert(item < (int)countof(material_list));
    scene->current_object->material->ambient = vec4(material_list[item].ambient, 1);
    scene->current_object->material->diffuse = vec4(material_list[item].diffuse, 1);
    scene->current_object->material->specular = vec4(material_list[item].specular, 1);
    scene->current_object->material->gloss = material_list[item].gloss;
    glutPostRedisplay();
}

/******************************************************************************/

static const struct {
    const char *name;
    vec3 color;
} light_list[] = {
    { "White", vec3(1) },
    { "Yellow white", vec3(1,1,0.8) },
    { "Bright white", vec3(1.8) },
    { "Red", vec3(1,0,0) },
    { "Green", vec3(0,1,0) },
    { "Blue", vec3(0,0,1) },
    { "Yellow", vec3(1,1,0) },
};

void lightMenu(int item)
{
    assert(item < (int)countof(light_list));
    light->setColor(light_list[item].color);
    glutPostRedisplay();
}

/******************************************************************************/

static const struct {
    const char *name;
    const char *filename;
} shader_list[] = {
    { "0 Red",                 "glsl/00_red.frag" },
    { "1 Decal",               "glsl/01_decal.frag" },
    { "2 Diffuse",             "glsl/02_diffuse.frag" },
    { "3 Bumpy diffuse",       "glsl/03_bump_diffuse.frag" },
    { "4 Specular",            "glsl/04_specular.frag" },
    { "5 Diffuse + specular",  "glsl/05_diffspec.frag" },
    { "6 Bump lit",            "glsl/06_bump_lit.frag" },
    { "7 Reflection",          "glsl/07_reflection.frag" },
    { "8 Bumpy reflection",    "glsl/08_bump_reflection.frag" },
    { "9 Combo",               "glsl/09_combo.frag" }
};

void shaderMenu(int item)
{
    assert(item < (int)countof(shader_list));
    const char *filename = shader_list[item].filename;
    printf("Switching to shader \"%s\", loaded from %s...\n", shader_list[item].name, filename);
    scene->current_object->fragment_filename = filename;
    scene->current_object->loadProgram();
    glutPostRedisplay();
}

/******************************************************************************/

void viewerMenu(int item)
{
    assert(item < (int)scene->viewer_list.size());
    scene->setViewer(scene->viewer_list[item]);
    scene->viewer->setPosition();
    glutPostRedisplay();
}

/******************************************************************************/

void fixView(ObjectPtr obj) {
    at_vector = vec3(obj->transform * vec4(obj->aabb.centroid, 1.0));
    float dist = max(max(obj->aabb.rtf.x - obj->aabb.lbn.x, obj->aabb.rtf.y - obj->aabb.lbn.y),
                     obj->aabb.rtf.z - obj->aabb.lbn.z);
    float speed = 0.05 * dist;
    eye_vector = vec3(obj->transform * vec4(0.0, 0.0, 3 * dist, 1.0));
    for (size_t i = 0; i < scene->viewer_list.size(); i++) {
        scene->viewer_list[i]->reset(eye_vector, at_vector);
        if (scene->viewer_list[i]->controller)
            scene->viewer_list[i]->controller->setSpeed(speed);
    }
    for (size_t i = 0; i < scene->light_list.size(); i++) {
        scene->light_list[i]->setCenter(at_vector);
        scene->light_list[i]->setRadius(dist);
    }
}

static const struct {
    const char *name;
} object_list[] = {
    { "Torus" },
};

void objectMenu(int item)
{
    assert(item < (int)countof(object_list));
    MaterialPtr material = MaterialPtr(new Material());
    if (item == 0 || item == 1) {
        TorusPtr torus = TorusPtr(new Torus(0.5, 1.5, material));
        scene->addObject(torus);
        scene->current_object = torus;
        materialMenu(7);
        bumpyMenu(8);
        decalMenu(0);
        //envMapMenu(0);
        torus->material->env_map = scene->env_map;
        shaderMenu(9);
        fixView(torus);
    }
    glutPostRedisplay();
}

struct fileListItem {
    const char *name;
    const char *basepath;
    const char *filename;

    fileListItem(const char* n, const char* p, const char* fn)
        : name(n), basepath(p), filename(fn)
    {}
};

vector<fileListItem> objFile_list;

void makeFileList(const string& pattern) {
    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    for(unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
        string result(glob_result.gl_pathv[i]);
        int pos = result.find("./");
        string filename = result.substr(pos + 2);
        pos = filename.rfind('/');
        string basepath = filename.substr(0, pos + 1);
        string name = filename;
        pos = name.rfind('.');
        name = name.substr(0, pos);
        pos = name.rfind('/');
        name = name.substr(pos + 1);
        char* nm = new char[name.length() + 1];
        strcpy(nm, name.c_str());
        char* bp = new char[basepath.length() + 1];
        strcpy(bp, basepath.c_str());
        char* fn = new char[filename.length() + 1];
        strcpy(fn, filename.c_str());
        objFile_list.push_back(fileListItem(nm, bp, fn));
    }
    globfree(&glob_result);
}

void objFileMenu(int item)
{
    assert(item < objFile_list.size());
    ObjFileObjectPtr obj = ObjFileObjectPtr(new ObjFileObject());
    obj->obj_basepath = objFile_list[item].basepath;
    obj->obj_filename = objFile_list[item].filename;
    if (obj->loadObjFile()) {
        scene->addObject(obj);
        scene->current_object = obj;
        fixView(obj);
    }
    glutPostRedisplay();
}

/******************************************************************************/

enum {
    M_STOP_ANIMATION,
    M_DELETE,
    M_QUIT
};


static void menu(int item)
{
    switch (item) {
    case M_QUIT:
        exit(0);
        break;
    case M_STOP_ANIMATION:
        if (scene->current_object.get())
        scene->obj_control->stopSpinning();
        glutPostRedisplay();
        break;
    case M_DELETE:
        for (ObjList::iterator i = scene->object_list.begin(); i < scene->object_list.end(); i++) {
            if (scene->current_object == (*i)) {
                scene->object_list.erase(i);
                if (!scene->object_list.empty()) scene->current_object = scene->object_list[0];
                else scene->current_object.reset();
            }
        }
        glutPostRedisplay();
        break;
    default:
        assert(!"unknown menu item");
        break;
    }
}

void initMenus()
{
    int decal_menu = glutCreateMenu(decalMenu);
    for (int i=0; i<countof(decal_list); i++) {
        glutAddMenuEntry(decal_list[i].name, i);
    }
    int bumpy_menu = glutCreateMenu(bumpyMenu);
    for (int i=0; i<countof(bumpy_list); i++) {
        glutAddMenuEntry(bumpy_list[i].name, i);
    }
    int material_menu = glutCreateMenu(materialMenu);
    for (int i=0; i<countof(material_list); i++) {
        glutAddMenuEntry(material_list[i].name, i);
    }
    int envmap_menu = glutCreateMenu(envMapMenu);
    for (int i=0; i<countof(envmap_list); i++) {
        glutAddMenuEntry(envmap_list[i].name, i);
    }
    int light_menu = glutCreateMenu(lightMenu);
    for (int i=0; i<countof(light_list); i++) {
        glutAddMenuEntry(light_list[i].name, i);
    }
    int shader_menu = glutCreateMenu(shaderMenu);
    for (int i=0; i<countof(shader_list); i++) {
        glutAddMenuEntry(shader_list[i].name, i);
    }
    int viewer_menu = glutCreateMenu(viewerMenu);
    for (int i = 0; i < scene->viewer_list.size(); i++) {
        glutAddMenuEntry(scene->viewer_list[i]->name, i);
    }
    makeFileList("./objects/*.obj");
    int objFile_menu = glutCreateMenu(objFileMenu);
    for (int i = 0; i < objFile_list.size(); i++) {
        glutAddMenuEntry(objFile_list[i].name, i);
    }
    int object_menu = glutCreateMenu(objectMenu);
    for (int i=0; i<countof(object_list); i++) {
        glutAddMenuEntry(object_list[i].name, i);
    }
    glutAddSubMenu("Obj Files...", objFile_menu);

    glutCreateMenu(menu);
    glutAddSubMenu("New Object...", object_menu);
    if (!scene->viewer_list.empty()) glutAddSubMenu("Camera...", viewer_menu);
    glutAddSubMenu("Decal texture...", decal_menu);
    glutAddSubMenu("Bump texture...", bumpy_menu);
    glutAddSubMenu("Material...", material_menu);
    glutAddSubMenu("Environments...", envmap_menu);
    glutAddSubMenu("Light color...", light_menu);
    glutAddSubMenu("Shader...", shader_menu);
    glutAddMenuEntry("Stop animation", M_STOP_ANIMATION);
    glutAddMenuEntry("Delete current object", M_DELETE);
    glutAddMenuEntry("Quit", M_QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

