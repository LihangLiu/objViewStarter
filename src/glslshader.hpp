#ifndef __glslshader_hpp__
#define __glslshader_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "global.hpp"

class GLSLShader {
public:
    GLenum shader_type;
    GLint bytes;
    GLchar* source;
    GLuint shader_object;
    bool dirty;

    GLSLShader(GLenum type, int len, const char* s);
    GLSLShader(GLenum type);
    ~GLSLShader();

    bool readTextFile(const char *filename);
    void validate();
    GLuint getShader();
    void showInfoLog(const char* msg);
    void reset();
    void release();
};
typedef shared_ptr<GLSLShader> GLSLShaderPtr;

class FragmentShader : public GLSLShader {
public:
    FragmentShader(int len, const char *s);
    FragmentShader();
};

class VertexShader : public GLSLShader {
public:
    VertexShader(int len, const char *s);
    VertexShader();
};

#endif // __glslshader_hpp__
