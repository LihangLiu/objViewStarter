#include "material.hpp"

#define DECAL 0x1
#define AMBIENT 0x2
#define DIFFUSE 0x4
#define SPECULAR 0x8
#define GLOSS 0x10
#define NORMAL 0x20
#define CUBE 0x40

Material::Material()
    : ambient(vec3(0.2), 1.0)
    , diffuse(vec3(0.8),1.0)
    , specular(vec3(0.0), 1.0)
    , gloss(0)
    , decal(0)
    , ambient_map(0)
    , diffuse_map(0)
    , specular_map(0)
    , gloss_map(0)
    , normal_map(0)
    , env_map(0)
    , decal_flag(false)
    , amb_flag(false)
    , diff_flag(false)
    , spec_flag(false)
    , gloss_flag(false)
    , bump_flag(false)
    , env_flag(false)
{}

void Material::bindTextures()
{
    if (decal.get()) decal->bind(GL_TEXTURE0);
    if (ambient_map.get()) ambient_map->bind(GL_TEXTURE1);
    if (diffuse_map.get()) diffuse_map->bind(GL_TEXTURE2);
    if (specular_map.get()) specular_map->bind(GL_TEXTURE3);
    if (gloss_map.get()) gloss_map->bind(GL_TEXTURE4);
    if (normal_map.get()) normal_map->bind(GL_TEXTURE5);
    if (env_map.get()) env_map->bind(GL_TEXTURE6);
}

void Material::setDecal(Texture2DPtr p)
{
    decal = p;
    decal_flag = true;
}

void Material::setAmbientMap(Texture2DPtr p)
{
    ambient_map = p;
    amb_flag = true;
}

void Material::setDiffuseMap(Texture2DPtr p)
{
    diffuse_map = p;
    diff_flag = true;
}

void Material::setSpecularMap(Texture2DPtr p)
{
    specular_map = p;
    spec_flag = true;
}

void Material::setGlossMap(Texture2DPtr p)
{
    gloss_map = p;
    gloss_flag = true;
}

void Material::setNormalMap(NormalMapPtr p)
{
    normal_map = p;
    bump_flag = true;
}

void Material::setCubeMap(CubeMapPtr p)
{
    env_map = p;
    env_flag = true;
}

Material& Material::operator =(const Material& rhs)
{
    if (this != &rhs) {
        ambient = rhs.ambient;
        diffuse = rhs.diffuse;
        specular = rhs.specular;
        gloss = rhs.gloss;
        decal = rhs.decal;
        ambient_map = rhs.ambient_map;
        diffuse_map = rhs.diffuse_map;
        specular_map = rhs.specular_map;
        gloss_map = rhs.gloss_map;
        normal_map = rhs.normal_map;
        env_map = rhs.env_map;
        decal_flag = rhs.decal_flag;
        amb_flag = rhs.amb_flag;
        diff_flag = rhs.diff_flag;
        spec_flag = rhs.spec_flag;
        gloss_flag = rhs.gloss_flag;
        bump_flag = rhs.bump_flag;
        env_flag = rhs.env_flag;
    }
    return *this;
}
