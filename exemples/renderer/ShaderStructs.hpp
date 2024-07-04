/*
 * ---------------------------------------------------
 * ShaderStructs.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/26 10:33:31
 * ---------------------------------------------------
 */

#ifndef SHADERSTRUCTS_HPP
# define SHADERSTRUCTS_HPP

#ifndef __METAL_VERSION__
    #include "Graphics/StructLayout.hpp"
#endif
#include "Math/Vector.hpp"
#include "UtilsCPP/Types.hpp"

#ifndef __METAL_VERSION__
namespace shaderStruct {
#endif

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
};

struct PointLight
{
    math::vec3f position;
    math::rgb   color;
    float       ambiantIntensity;
    float       diffuseIntensity;
    float       specularIntensity;
};

namespace universal
{
    struct Material
    {
        math::rgb     ambientColor;
        utils::uint32 useAmbientTexture;
        
        math::rgb     diffuseColor;
        utils::uint32 useDiffuseTexture;
        
        math::rgb     specularColor;
        utils::uint32 useSpecularTexture;
        
        math::rgb     emissiveColor;
        utils::uint32 useEmissiveTexture;
        
        float         shininess;
        utils::uint32 useShininessTexture;
    };
}

namespace baseColor
{
    struct Material
    {
        math::rgb baseColor;
        math::rgb specularColor;
        math::rgb emissiveColor;
        float     shininess;
    };
}

namespace baseTexture
{
    struct Material
    {
        math::rgb specularColor;
        math::rgb emissiveColor;
        float     shininess;
    };
}

#ifndef __METAL_VERSION__
} // namespace shaderStruct
#endif

#ifndef __METAL_VERSION__
template<>
inline gfx::StructLayout gfx::getLayout<shaderStruct::PointLight>()
{
    return {
        { "position",          Type::vec3f, (void*)offsetof(shaderStruct::PointLight, position)          },
        { "color",             Type::vec3f, (void*)offsetof(shaderStruct::PointLight, color)             },
        { "ambiantIntensity",  Type::Float, (void*)offsetof(shaderStruct::PointLight, ambiantIntensity)  },
        { "diffuseIntensity",  Type::Float, (void*)offsetof(shaderStruct::PointLight, diffuseIntensity)  },
        { "specularIntensity", Type::Float, (void*)offsetof(shaderStruct::PointLight, specularIntensity) }
    };
}

template<>
inline gfx::StructLayout gfx::getLayout<shaderStruct::universal::Material>()
{
    return {
        { "ambientColor",        Type::vec3f,  (void*)offsetof(shaderStruct::universal::Material, ambientColor)        },
        { "useAmbientTexture",   Type::Uint32, (void*)offsetof(shaderStruct::universal::Material, useAmbientTexture)   },
        { "diffuseColor",        Type::vec3f,  (void*)offsetof(shaderStruct::universal::Material, diffuseColor)        },
        { "useDiffuseTexture",   Type::Uint32, (void*)offsetof(shaderStruct::universal::Material, useDiffuseTexture)   },
        { "specularColor",       Type::vec3f,  (void*)offsetof(shaderStruct::universal::Material, specularColor)       },
        { "useSpecularTexture",  Type::Uint32, (void*)offsetof(shaderStruct::universal::Material, useSpecularTexture)  },
        { "emissiveColor",       Type::vec3f,  (void*)offsetof(shaderStruct::universal::Material, emissiveColor)       },
        { "useEmissiveTexture",  Type::Uint32, (void*)offsetof(shaderStruct::universal::Material, useEmissiveTexture)  },
        { "shininess",           Type::Float,  (void*)offsetof(shaderStruct::universal::Material, shininess)           },
        { "useShininessTexture", Type::Uint32, (void*)offsetof(shaderStruct::universal::Material, useShininessTexture) }
    };
}

template<>
inline gfx::StructLayout gfx::getLayout<shaderStruct::baseColor::Material>()
{
    return {
        { "baseColor",     Type::vec3f, (void*)offsetof(shaderStruct::baseColor::Material, baseColor)     },
        { "specularColor", Type::vec3f, (void*)offsetof(shaderStruct::baseColor::Material, specularColor) },
        { "emissiveColor", Type::vec3f, (void*)offsetof(shaderStruct::baseColor::Material, emissiveColor) },
        { "shininess",     Type::Float, (void*)offsetof(shaderStruct::baseColor::Material, shininess)     }
    };
}

template<>
inline gfx::StructLayout gfx::getLayout<shaderStruct::baseTexture::Material>()
{
    return {
        { "specularColor", Type::vec3f, (void*)offsetof(shaderStruct::baseTexture::Material, specularColor) },
        { "emissiveColor", Type::vec3f, (void*)offsetof(shaderStruct::baseTexture::Material, emissiveColor) },
        { "shininess",     Type::Float, (void*)offsetof(shaderStruct::baseTexture::Material, shininess)     }
    };
}
#endif

#endif // SHADERSTRUCTS_HPP