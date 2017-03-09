#ifndef __texture_hpp__
#define __texture_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "global.hpp"

class TextureImage {
public:
    char *filename;
    int width, height, components, orig_components;
    unsigned char *image;

    TextureImage();
    TextureImage(const char *filename);
    ~TextureImage();

    bool load();

protected:
    bool loadImage();
    void verticalFlip();
};

/******************************************************************************/

class TextureGLState {
private:
    bool mipmapped;
    GLuint texture_object;

public:
    TextureGLState();
    ~TextureGLState();

    void tellGL();
    GLuint getTextureObject();
    bool isMipmapped();
};

/******************************************************************************/

class Texture : public TextureImage, public TextureGLState {
public:
    Texture(const char *filename);
    ~Texture();
};
typedef shared_ptr<Texture> TexturePtr;

/******************************************************************************/

class Texture2D : public Texture {
public:
    static const GLenum target = GL_TEXTURE_2D;

    Texture2D(const char *filename);
    ~Texture2D();

    void tellGL();
    void bind();
    void bind(GLenum texture_unit);
};
typedef shared_ptr<Texture2D> Texture2DPtr;

/******************************************************************************/

class NormalMap : public Texture2D {
private:
    GLubyte* normal_image;
    vec3 computeNormal(int i, int j, float scale);

public:
    NormalMap(const char *filename);
    ~NormalMap();

    bool load(float scale);
    void tellGL();
};
typedef shared_ptr<NormalMap> NormalMapPtr;

/******************************************************************************/

class CubeMap : public TextureGLState {
private:
    static const GLenum target = GL_TEXTURE_CUBE_MAP;
    TextureImage face[6];

public:
    CubeMap(const char *filename);
    ~CubeMap();

    bool load();
    void tellGL();
    void bind();
    void bind(GLenum texture_unit);
    void draw(float scale);
};
typedef shared_ptr<CubeMap> CubeMapPtr;

#endif // __texture_hpp__
