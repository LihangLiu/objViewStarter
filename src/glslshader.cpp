#include "glslshader.hpp"

extern bool verbose;

GLSLShader::GLSLShader(GLenum type, int len, const char* s)
    : shader_type(type)
    , bytes(len)
    , shader_object(0)
    , dirty(true)
{
    source = new GLchar[bytes];
    if (source) {
        for (int i=0; i<bytes; i++) {
            source[i] = s[i];
        }
    }
}

GLSLShader::GLSLShader(GLenum type)
    : shader_type(type)
    , source(NULL)
    , shader_object(0)
    , dirty(true)
{}

extern const char* program_name;

bool GLSLShader::readTextFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file) {
        long size;

        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        fseek(file, 0L, SEEK_SET);
        delete source;
        source = new GLchar[size];
        if (source == NULL) {
            fprintf(stderr, "%s: malloc failed\n", program_name);
            return false;
        }
        bytes = (int) fread(source, 1, size, file);
        assert(bytes == size);
        fclose(file);
        dirty = true;
    } else {
        fprintf(stderr, "%s: could not open file %s\n",
            program_name, filename);
        return false;
    }
    return true;
}

void GLSLShader::validate()
{
    if (dirty || !shader_object) {
        if (!shader_object) {
            shader_object = glCreateShader(shader_type);
        }
        if (shader_object) {
            const GLchar** s = (const GLchar**) &source;
            glShaderSource(shader_object, 1, s, &bytes);
            glCompileShader(shader_object);
            dirty = false;
            GLint compiled = 0;
            glGetShaderiv(shader_object, GL_COMPILE_STATUS, &compiled);
            if (verbose || !compiled) {
                showInfoLog("shader");
            }
        }
    }
}

GLuint GLSLShader::getShader()
{
    validate();
    return shader_object;
}

void GLSLShader::showInfoLog(const char* msg)
{
    validate();

    GLint max_length, length;
    GLchar *info_log;

    glGetShaderiv(shader_object,
        GL_INFO_LOG_LENGTH, &max_length);
    if (max_length > 1) {
        info_log = (GLchar *) malloc(max_length);
        if (info_log != NULL) {
            glGetShaderInfoLog(shader_object, max_length, &length, info_log);
            printf("=== %s info log contents ===\n", msg);
            printf("%s", info_log);
            printf("=== end ===\n");
            free(info_log);
        }
    } else {
        printf("=== %s has an <empty> info log (a good thing) ===\n", msg);
    }
}

void GLSLShader::reset()
{
    if (shader_object) {
        if (verbose) {
            printf("Deleting shader 0x%x\n", shader_object);
        }
        glDeleteShader(shader_object);
        shader_object = 0;
    }
}

void GLSLShader::release()
{
    shader_object = 0;
}

GLSLShader::~GLSLShader()
{
    reset();
    delete source;
}

FragmentShader::FragmentShader(int len, const char *s)
    : GLSLShader(GL_FRAGMENT_SHADER, len, s)
{}

FragmentShader::FragmentShader()
    : GLSLShader(GL_FRAGMENT_SHADER)
{}

VertexShader::VertexShader(int len, const char *s)
    : GLSLShader(GL_VERTEX_SHADER, len, s)
{}

VertexShader::VertexShader()
    : GLSLShader(GL_VERTEX_SHADER)
{}

