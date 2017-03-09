#ifndef __glslprogram_hpp__
#define __glslprogram_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "global.hpp"

class GLSLProgram {
public:
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program_object;
    bool dirty;

    GLSLProgram(GLuint vertex_shader, GLuint fragment_shader);
    GLSLProgram();
    void showInfoLog(const char* msg);
    bool validate();
    void use();
    void reset();
    void swap(GLSLProgram &other);
    void setVec1i(const char *name, int v);
    void setVec1f(const char *name, float v);
    void setVec2f(const char *name, vec2 v);
    void setVec3f(const char *name, vec3 v);
    void setVec4f(const char *name, vec4 v);
    void setMat4f(const char *name, mat4x4 transform);
    void setSampler(const char *name, int texture_unit);
    GLint getLocation(const char *name);
    ~GLSLProgram();
};

#endif // __glslprogram_hpp__
