#include "glslprogram.hpp"

extern bool verbose;

GLSLProgram::GLSLProgram(GLuint vs, GLuint fs)
    : vertex_shader(vs)
    , fragment_shader(fs)
    , program_object(0)
    , dirty(true)
{}

GLSLProgram::GLSLProgram()
    : vertex_shader(0)
    , fragment_shader(0)
    , program_object(0)
    , dirty(true)
{}

#define ERR_CHECK() glutReportErrors(); printf("%s: %d\n", __FILE__, __LINE__);

bool GLSLProgram::validate()
{
    if (dirty || !program_object) {
        if (!program_object) {
            program_object = glCreateProgram();
            assert(program_object);  // API failure!
        }
        if (program_object) {
            glAttachShader(program_object, vertex_shader);
            glAttachShader(program_object, fragment_shader);
            glLinkProgram(program_object);
            dirty = false;
            GLint linked = 0;
            glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
            if (verbose || !linked) {
                showInfoLog("linked program");
                if (!linked) return false;
            }
            assert(linked == GL_TRUE);
        }
    }
    return true;
}

void GLSLProgram::showInfoLog(const char* msg)
{
    validate();

    GLint max_length, length;
    GLchar *info_log;

    glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &max_length);
    if (max_length > 1) {
        info_log = (GLchar *) malloc(max_length);
        if (info_log != NULL) {
            glGetProgramInfoLog(program_object, max_length, &length, info_log);
            printf("=== %s info log contents ===\n", msg);
            printf("%s", info_log);
            printf("=== end ===\n");
            free(info_log);
        }
    } else {
        printf("=== %s has an <empty> info log (a good thing) ===\n", msg);
    }
}

void GLSLProgram::use()
{
    validate();
    assert(program_object);
    glUseProgram(program_object);
}

void GLSLProgram::reset()
{
    if (program_object) {
        glDeleteProgram(program_object);
        program_object = 0;
    }
    if (vertex_shader) {
        if (verbose) {
            printf("Deleting vertex shader 0x%x\n", vertex_shader);
        }
        glDeleteShader(vertex_shader);
        vertex_shader = 0;
    }
    if (fragment_shader) {
        if (verbose) {
            printf("Deleting fragment shader 0x%x\n", fragment_shader);
        }
        glDeleteShader(fragment_shader);
        fragment_shader = 0;
    }
}

void GLSLProgram::swap(GLSLProgram &other)
{
    GLuint tmp;

    tmp = program_object;
    program_object = other.program_object;
    other.program_object = tmp;

    tmp = vertex_shader;
    vertex_shader = other.vertex_shader;
    other.vertex_shader = tmp;

    tmp = fragment_shader;
    fragment_shader = other.fragment_shader;
    other.fragment_shader = tmp;

    bool btmp;
    btmp = dirty;
    dirty = other.dirty;
    other.dirty = btmp;
}
void GLSLProgram::setVec1i(const char *name, int v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform1i(loc, v);
    }
}

void GLSLProgram::setVec1f(const char *name, float v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform1f(loc, v);
    }
}

void GLSLProgram::setVec2f(const char *name, vec2 v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform2f(loc, v.x, v.y);
    }
}

void GLSLProgram::setVec3f(const char *name, vec3 v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform3f(loc, v.x, v.y, v.z);
    }
}

void GLSLProgram::setVec4f(const char *name, vec4 v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
}

void GLSLProgram::setMat4f(const char *name, mat4x4 transform)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        mat3x3 m = mat3x3(transform);
        GLboolean needs_transpose = GL_FALSE;
        glUniformMatrix3fv(loc, 1, needs_transpose, &m[0][0]);
    }
}

void GLSLProgram::setSampler(const char *name, int texture_unit)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform1i(loc, texture_unit);
    }
}

extern const char* program_name;

GLint GLSLProgram::getLocation(const char *name)
{
  GLint location = glGetUniformLocationARB(program_object, name);

  if (location < 0) {
    fprintf(stderr, "%s: could not get location of %s (fix your shader so it is used)\n",
      program_name, name);
  }
  return location;
}

GLSLProgram::~GLSLProgram()
{
    reset();
}
