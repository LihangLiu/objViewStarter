#include "texture.hpp"
#include "imageload/stb_image.h"

extern const char *program_name;
extern bool verbose;

TextureImage::TextureImage()
    : filename(NULL)
    , width(0)
    , height(0)
    , components(0)
    , orig_components(0)
    , image(NULL)
{}

TextureImage::TextureImage(const char *fn)
    : width(0)
    , height(0)
    , components(0)
    , orig_components(0)
    , image(NULL)
{
    filename = new char[strlen(fn)+1];
    assert(filename);
    strcpy(filename, fn);
}

TextureImage::~TextureImage()
{
    stbi_image_free(image);
    delete filename;
}

bool TextureImage::load()
{
    components = 4;
    return loadImage();
}

bool TextureImage::loadImage()
{
    image = stbi_load(filename, &width, &height, &orig_components, components);
    if (image) {
        string fn(filename);
        if (fn.rfind(".jpg") != string::npos
            || fn.rfind(".jpeg") != string::npos
            || fn.rfind(".JPG") != string::npos
            || fn.rfind(".JPEG") != string::npos)
        {
            verticalFlip();
        }
        return true;
    } else {
        printf("%s: failed to load image %s\n", program_name, filename);
        return false;
    }
}

void TextureImage::verticalFlip()
{
    int stride = width * components;
    int top = 0;
    int bottom = stride * (height - 1);
    unsigned char buffer;
    while (top < bottom) {
        for (int i = 0; i < stride; i++) {
            unsigned char* target1 = image + top + i;
            unsigned char* target2 = image + bottom + i;
            buffer = *target1;
            *target1 = *target2;
            *target2 = buffer;
        }
        top += stride;
        bottom -= stride;
    }
}

/******************************************************************************/

TextureGLState::TextureGLState()
    : mipmapped(true)
    , texture_object(0)
{
}

TextureGLState::~TextureGLState()
{
    if (texture_object) {
        glDeleteTextures(1, &texture_object);
    }
}

void TextureGLState::tellGL()
{
    if (texture_object == 0) {
        glGenTextures(1, &texture_object);
    }
}

GLuint TextureGLState::getTextureObject()
{
    return texture_object;
}

bool TextureGLState::isMipmapped()
{
    return mipmapped;
}

/******************************************************************************/
Texture::Texture(const char *fn)
    : TextureImage(fn)
{
}

Texture::~Texture()
{
}

/******************************************************************************/

Texture2D::Texture2D(const char *fn)
    : Texture(fn)
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::bind()
{
    glBindTexture(target, getTextureObject());
}

void Texture2D::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(target, getTextureObject());
}

void Texture2D::tellGL()
{
    TextureGLState::tellGL();

    bind();
    glTexImage2D(target, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    if (isMipmapped()) {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

/******************************************************************************/

NormalMap::NormalMap(const char *fn)
    : Texture2D(fn)
    , normal_image(NULL)
{}

struct PackedNormal {
    GLubyte n[3];

    PackedNormal(vec3 normal);
};

PackedNormal::PackedNormal(vec3 normal)
{
    assert(normal.x >= -1);
    assert(normal.y >= -1);
    assert(normal.z >= -1);

    assert(normal.x <= 1);
    assert(normal.y <= 1);
    assert(normal.z <= 1);
    n[0] = 128 + 127 * normal.x;
    n[1] = 128 + 127 * normal.y;
    n[2] = 128 + 127 * normal.z;
}

vec3 NormalMap::computeNormal(int i, int j, float scale)
{
    float x = image[j*width+i];
    float top = image[((j-1)*width+i)%(width*height)];
    float left = image[(j*width+i-1)%(width*height)];
    vec3 normal = vec3((x-top)*scale/255.0, (x-left)*scale/255.0, 1);
    normal = normalize(normal);
    // printf("%f %f %f\n", normal.x, normal.y, normal.z);
    return normal;
}

bool NormalMap::load(float scale)
{
    components = 1;
    if(loadImage()) {
        assert(width > 0);
        assert(height > 0);
        assert(orig_components > 0);
        if (orig_components != components) {
            if (verbose) {
                printf("warning: %d component normal map treated as gray scale height map\n",
                    orig_components);
            }
        }
        normal_image = new GLubyte[width * height * 3];
        assert(normal_image);
        GLubyte* p = normal_image;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                vec3 normal = computeNormal(x, y, scale);
                PackedNormal packed_normal(normal);
                p[0] = packed_normal.n[0];
                p[1] = packed_normal.n[1];
                p[2] = packed_normal.n[2];
                p += 3;
            }
        }
        assert(p == normal_image + (width * height * 3));
        // success
        return true;
    } else {
        printf("%s: failed to load image %s\n", program_name, filename);
        return false;
    }
}

void NormalMap::tellGL()
{
    TextureGLState::tellGL();

    bind();
    glTexImage2D(target, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, normal_image);
    if (isMipmapped()) {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

NormalMap::~NormalMap()
{
    delete normal_image;
}

/******************************************************************************/

static const char *face_name[6] = {
    "xpos",
    "xneg",
    "ypos",
    "yneg",
    "zpos",
    "zneg"
};

CubeMap::CubeMap(const char *filename_pattern)
{
    for (int i = 0; i < 6; i++) {
        char buffer[200];

        snprintf(buffer, sizeof(buffer), filename_pattern, face_name[i]);
        face[i].filename = new char[strlen(buffer)+1];
        strcpy(face[i].filename, buffer);
    }
}

CubeMap::~CubeMap()
{
}

bool CubeMap::load()
{
    bool success = true;
    for (int i = 0; i < 6; i++) {
        success &= face[i].load();
    }
    return success;
}

void CubeMap::tellGL()
{
    TextureGLState::tellGL();

    bind();

    GLint base_level = 0;
    for (int i = 0; i < 6; i++) {
        TextureImage &img = face[i];

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, base_level,
            GL_RGBA8, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image);
    }
    if (isMipmapped()) {
        glGenerateMipmap(target);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void CubeMap::bind()
{
    glBindTexture(target, getTextureObject());
}

void CubeMap::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(target, getTextureObject());
}

static const GLfloat vertex[4*6][3] = {
    /* Positive X face. */
    { 1, -1, -1 },  { 1, 1, -1 },  { 1, 1, 1 },  { 1, -1, 1 },
    /* Negative X face. */
    { -1, -1, -1 },  { -1, 1, -1 },  { -1, 1, 1 },  { -1, -1, 1 },
    /* Positive Y face. */
    { -1, 1, -1 },  { 1, 1, -1 },  { 1, 1, 1 },  { -1, 1, 1 },
    /* Negative Y face. */
    { -1, -1, -1 },  { 1, -1, -1 },  { 1, -1, 1 },  { -1, -1, 1 },
    /* Positive Z face. */
    { -1, -1, 1 },  { 1, -1, 1 },  { 1, 1, 1 },  { -1, 1, 1 },
    /* Negative Z face. */
    { -1, -1, -1 },  { 1, -1, -1 },  { 1, 1, -1 },  { -1, 1, -1 },
};

void CubeMap::draw(float scale)
{
    GLenum texture_unit = GL_TEXTURE3;
    glUseProgram(0);

    bind(texture_unit);
    glActiveTexture(texture_unit);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(scale, scale, scale);
    glBegin(GL_QUADS);
        /* For each vertex of each face of the cube... */
        for (int i = 0; i < 4*6; i++) {
            glMultiTexCoord3fv(texture_unit, vertex[i]);
            glVertex3f(vertex[i][0], vertex[i][1], vertex[i][2]);
        }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_CUBE_MAP);
    glActiveTexture(GL_TEXTURE0);
}
