#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "object.hpp"
#include <iostream>

/******************************************************************************/

void Object::drawAabb() {
    glUseProgram(0); // Fixed function
    glPushMatrix();
    glMultMatrixf(value_ptr(transform));
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(aabb.lbn.x, aabb.lbn.y, aabb.lbn.z);
    glVertex3f(aabb.rtf.x, aabb.lbn.y, aabb.lbn.z);
    glVertex3f(aabb.rtf.x, aabb.rtf.y, aabb.lbn.z);
    glVertex3f(aabb.lbn.x, aabb.rtf.y, aabb.lbn.z);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(aabb.lbn.x, aabb.rtf.y, aabb.rtf.z);
    glVertex3f(aabb.rtf.x, aabb.rtf.y, aabb.rtf.z);
    glVertex3f(aabb.rtf.x, aabb.lbn.y, aabb.rtf.z);
    glVertex3f(aabb.lbn.x, aabb.lbn.y, aabb.rtf.z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(aabb.lbn.x, aabb.lbn.y, aabb.lbn.z);
    glVertex3f(aabb.lbn.x, aabb.lbn.y, aabb.rtf.z);
    glVertex3f(aabb.rtf.x, aabb.lbn.y, aabb.lbn.z);
    glVertex3f(aabb.rtf.x, aabb.lbn.y, aabb.rtf.z);
    glVertex3f(aabb.rtf.x, aabb.rtf.y, aabb.lbn.z);
    glVertex3f(aabb.rtf.x, aabb.rtf.y, aabb.rtf.z);
    glVertex3f(aabb.lbn.x, aabb.rtf.y, aabb.lbn.z);
    glVertex3f(aabb.lbn.x, aabb.rtf.y, aabb.rtf.z);
    glEnd();
    glPopMatrix();
    glutPostRedisplay();
}

void Object::highlight(bool h)
{
    highlighted = h;
    glutPostRedisplay();
}

void Object::makePlanes()
{
    if (aabb.lbn.x != aabb.rtf.x
        || aabb.lbn.y != aabb.rtf.y
        || aabb.lbn.z != aabb.rtf.z)
    {
        aabb.planes[0] = makePlane(aabb.lbn,
                    vec3(aabb.lbn.x, aabb.lbn.y, aabb.rtf.z),
                    vec3(aabb.lbn.x, aabb.rtf.y, aabb.lbn.z));
        aabb.planes[1] = makePlane(aabb.lbn,
                    vec3(aabb.rtf.x, aabb.lbn.y, aabb.lbn.z),
                    vec3(aabb.lbn.x, aabb.lbn.y, aabb.rtf.z));
        aabb.planes[2] = makePlane(aabb.lbn,
                    vec3(aabb.lbn.x, aabb.rtf.y, aabb.lbn.z),
                    vec3(aabb.rtf.x, aabb.lbn.y, aabb.lbn.z));
        aabb.planes[3] = makePlane(aabb.rtf,
                    vec3(aabb.rtf.x, aabb.lbn.y, aabb.rtf.z),
                    vec3(aabb.rtf.x, aabb.rtf.y, aabb.lbn.z));
        aabb.planes[4] = makePlane(aabb.rtf,
                    vec3(aabb.rtf.x, aabb.rtf.y, aabb.lbn.z),
                    vec3(aabb.lbn.x, aabb.rtf.y, aabb.rtf.z));
        aabb.planes[5] = makePlane(aabb.rtf,
                    vec3(aabb.lbn.x, aabb.rtf.y, aabb.rtf.z),
                    vec3(aabb.rtf.x, aabb.lbn.y, aabb.rtf.z));
    }
}

PlanePtr Object::makePlane(vec3 p0, vec3 p1, vec3 p2)
{
    PlanePtr plane = PlanePtr(new(Plane));
    plane->n = normalize(cross(p1 - p0, p2 - p0));
    plane->d = dot(plane->n, p0);
    return plane;
}

/******************************************************************************/

Torus::Torus(float i, float o) : innerRad(i), outerRad(o)
{
    init();
}

Torus::Torus(float i, float o, MaterialPtr m) : innerRad(i), outerRad(o), Object(m)
{
    init();
}

Torus::~Torus() {
}

void Torus::makeAabb() {
    float lsize = innerRad + outerRad;
    aabb.lbn = vec3(-lsize, -lsize, -innerRad);
    aabb.rtf = vec3(lsize, lsize, innerRad);
    aabb.centroid = (aabb.lbn + aabb.rtf) / 2.0f;
    makePlanes();
}

void Torus::init()
{
    vertex_filename = "glsl/torus.vert";
    fragment_filename = "glsl/00_red.frag";
    mesh2d = Mesh2DPtr(new Mesh2D(vec2(0,0), vec2(1,1), ivec2(80,40)));
    loadProgram();
    makeAabb();
    curquat = quat(1.0f, 0.0f, 0.0f, 0.0f);
    curtrans = vec3(0);
    lasttrans = vec3(0);
}

void Torus::setMaterial(MaterialPtr m)
{
     material = m;
}

void Torus::loadProgram()
{
    VertexShader vs;
    FragmentShader fs;
    bool vs_ok = vs.readTextFile(vertex_filename.c_str());
    bool fs_ok = fs.readTextFile(fragment_filename.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs.getShader(), fs.getShader());
        vs.release();
        fs.release();
        bool ok = new_program.validate();
            if (ok) {
            program.swap(new_program);
            glBindAttribLocation(program.program_object, 0, "parametric");
            glLinkProgram(program.program_object);
            GLint torusInfo_location = program.getLocation("torus_info");
            program.use();
            glUniform2f(torusInfo_location, outerRad, innerRad);

            // Assign samplers statically to texture units 0 through 3
            program.setSampler("decal", 0);
            program.setSampler("normal_map", 5);
            program.setSampler("env_map", 6);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }
}

void Torus::draw(ViewerPtr viewer, LightPtr light)
{
    if (highlighted) drawAabb();
    program.use();

    vec3 eye_coords = vec3(inverse(transform) * vec4(viewer->eye_position, 1));
    program.setVec3f("eye_position", eye_coords);
    //program.setVec3f("eye_position", viewer->eye_position);

    vec3 light_coords = vec3(inverse(transform) * light->getPosition());
    program.setVec3f("light_position", light_coords);
    //program.setVec3f("light_position", light->getPosition());

    // lm = Light color modulated by Material color
    // a,d,s = ambient, diffuse, specular
    vec4 lma = material->ambient * light->getColor();
    program.setVec4f("lma", lma);
    vec4 lmd = material->diffuse * light->getColor();
    program.setVec4f("lmd", lmd);
    vec4 lms = material->specular * light->getColor();
    program.setVec4f("lms", lms);
    program.setVec1f("gloss", material->gloss);

    program.setMat4f("object_to_world", transform);
    //ERR_CHECK();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(value_ptr(transform));
    material->bindTextures();
    mesh2d->draw();
    glPopMatrix();
}

void Torus::idle()
{
    if (do_idle) {
        curquat = normalize(lastquat * curquat);
        transform = translate(mat4x4(1), curtrans);
        transform = transform * mat4_cast(curquat);
        transform = transform * baseMat;
        
        glutPostRedisplay();
    }
}

/******************************************************************************/

ObjFileObject:: ObjFileObject()
{
    init();
}

void ObjFileObject::makeAabb() {
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->makeAabb();
        if (meshes[i]->aabb.lbn.x < aabb.lbn.x) aabb.lbn.x = meshes[i]->aabb.lbn.x;
        if (meshes[i]->aabb.rtf.x > aabb.rtf.x) aabb.rtf.x = meshes[i]->aabb.rtf.x;
        if (meshes[i]->aabb.lbn.y < aabb.lbn.y) aabb.lbn.y = meshes[i]->aabb.lbn.y;
        if (meshes[i]->aabb.rtf.y > aabb.rtf.y) aabb.rtf.y = meshes[i]->aabb.rtf.y;
        if (meshes[i]->aabb.lbn.z < aabb.lbn.z) aabb.lbn.z = meshes[i]->aabb.lbn.z;
        if (meshes[i]->aabb.rtf.z > aabb.rtf.z) aabb.rtf.z = meshes[i]->aabb.rtf.z;
    }
    aabb.centroid = (aabb.lbn + aabb.rtf) / 2.0f;
    makePlanes();
}

void ObjFileObject::init()
{
    vertex_filename = "glsl/phong.vert";
    fragment_filename = "glsl/phong.frag";
    loadProgram();
    curquat = quat(1.0f, 0.0f, 0.0f, 0.0f);
    curtrans = vec3(0);
    lasttrans = vec3(0);
}

void ObjFileObject::loadProgram()
{
    VertexShader vs;
    FragmentShader fs;
    bool vs_ok = vs.readTextFile(vertex_filename.c_str());
    bool fs_ok = fs.readTextFile(fragment_filename.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs.getShader(), fs.getShader());
        vs.release();
        fs.release();
        bool ok = new_program.validate();
            if (ok) {
            program.swap(new_program);
            glBindAttribLocation(program.program_object, 0, "vertex");
            glBindAttribLocation(program.program_object, 1, "normal");
            glBindAttribLocation(program.program_object, 2, "texCoords");
            glLinkProgram(program.program_object);
            program.use();
            // Assign samplers statically to texture units
            program.setSampler("decal", 0);
            program.setSampler("ambient_map", 1);
            program.setSampler("diffuse_map", 2);
            program.setSampler("specular_map", 3);
            program.setSampler("gloss_map", 4);
            program.setSampler("normal_map", 5);
            program.setSampler("env_map", 6);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }
}

bool ObjFileObject::loadObjFile()
{
  std::cout << "Loading " << obj_filename << std::endl;
  vector<tinyobj::shape_t> shapes;
  vector<tinyobj::material_t> mtrls;
  string err;
  bool ret = tinyobj::LoadObj(shapes, mtrls, err, obj_filename, obj_basepath);

  if (!err.empty()) std::cerr << err << std::endl;
  if (!ret) return false;

  for (unsigned i = 0; i < mtrls.size(); ++i) makeMaterial(mtrls[i]);
  if (materials.empty()) materials.push_back(MaterialPtr(new Material()));
  for (unsigned i = 0; i < shapes.size(); ++i) makeMesh(shapes[i]);
  makeAabb();
  return true;
}

void ObjFileObject::makeMaterial(tinyobj::material_t m)
{
    MaterialPtr mtl = MaterialPtr(new Material());
    for (GLuint i = 0; i < 3; i++) {
        mtl->diffuse[i] = m.diffuse[i];
        mtl->ambient[i] = m.ambient[i];
        mtl->specular[i] = m.specular[i];
    }
    if (!m.diffuse_texname.empty()) {
        string fn = "objects/" + m.diffuse_texname;
        const char *filename = fn.c_str();
        Texture2DPtr diffMap(new Texture2D(filename));
        diffMap->load();
        diffMap->tellGL();
        mtl->setDiffuseMap(diffMap);
    }
    mtl->gloss = m.gloss;
    materials.push_back(mtl);
}

void ObjFileObject::makeMesh(tinyobj::shape_t s)
{
    meshes.push_back(ObjMeshPtr(new ObjMesh(s)));
}

void ObjFileObject::draw(ViewerPtr viewer, LightPtr light)
{
    if (highlighted) drawAabb();
    program.use();
    program.setVec3f("light_position", vec3(viewer->getViewMatrix() * light->getPosition()));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(value_ptr(transform));
    for (size_t i = 0; i < meshes.size(); ++i) {
        // lm = Light color modulated by Material color
        // a,d,s = ambient, diffuse, specular
        size_t idx = meshes[i]->index;
        vec4 lma = materials[idx]->ambient * light->getColor();
        program.setVec4f("lma", lma);
        vec4 lmd = materials[idx]->diffuse * light->getColor();
        program.setVec4f("lmd", lmd);
        vec4 lms = materials[idx]->specular * light->getColor();
        program.setVec4f("lms", lms);
        program.setVec1f("gloss", materials[idx]->gloss);
        program.setVec1i("amb_flag", materials[idx]->amb_flag);
        program.setVec1i("diff_flag", materials[idx]->diff_flag);
        program.setVec1i("spec_flag", materials[idx]->spec_flag);
        program.setVec1i("gloss_flag", materials[idx]->gloss_flag);
        program.setVec1i("bump_flag", materials[idx]->bump_flag);
        program.setVec1i("env_flag", materials[idx]->env_flag);
        materials[idx]->bindTextures();
        meshes[i]->draw();
    }
    glPopMatrix();
}

void ObjFileObject::idle()
{
    if (do_idle) {
        curquat = normalize(lastquat * curquat);
        transform = translate(mat4x4(1), curtrans);
        transform = transform * mat4_cast(curquat);
        transform = transform * baseMat;
        glutPostRedisplay();
    }
}
