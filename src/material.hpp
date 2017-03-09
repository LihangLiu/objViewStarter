#ifndef __material_hpp__
#define __material_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
#include "texture.hpp"

class Material {
public:
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float gloss;
    
    bool decal_flag;
    bool amb_flag;
    bool diff_flag;
    bool spec_flag;
    bool gloss_flag;
    bool bump_flag;
    bool env_flag;

    Texture2DPtr decal;
    Texture2DPtr ambient_map;
    Texture2DPtr diffuse_map;
    Texture2DPtr specular_map;
    Texture2DPtr gloss_map;
    NormalMapPtr normal_map;
    CubeMapPtr env_map;

    Material();
    Material& operator =(const Material& rhs);

    void bindTextures();
    void setDecal(Texture2DPtr p);
    void setAmbientMap(Texture2DPtr p);
    void setDiffuseMap(Texture2DPtr p);
    void setSpecularMap(Texture2DPtr p);
    void setGlossMap(Texture2DPtr p);
    void setNormalMap(NormalMapPtr p);
    void setCubeMap(CubeMapPtr p);
};
typedef shared_ptr<Material> MaterialPtr;

#endif // __material_hpp__
