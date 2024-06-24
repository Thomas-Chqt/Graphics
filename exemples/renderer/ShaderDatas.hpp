/*
 * ---------------------------------------------------
 * ShaderDatas.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 13:57:25
 * ---------------------------------------------------
 */

#ifndef SHADERDATAS_HPP
# define SHADERDATAS_HPP

#include "Math/Vector.hpp"
#include "UtilsCPP/Types.hpp"

#ifndef __METAL_VERSION__
    #include "Graphics/StructLayout.hpp"
#endif

#ifndef __METAL_VERSION__
namespace shaderData {
#endif

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
};

struct Material
{
    math::rgb     ambiant;
    math::rgb     diffuse;
    math::rgb     specular;
    math::rgb     emissive;
    float         shininess;
    utils::uint32 useDiffuseMap;
};

struct PointLight
{
    math::vec3f position;
    math::rgb   color;
    float       ambiantIntensity;
    float       diffuseIntensity;
    float       specularIntensity;
};

#ifndef __METAL_VERSION__
} // namespace shaderData
#endif

#ifndef __METAL_VERSION__
template<>
inline gfx::StructLayout gfx::getLayout<shaderData::Vertex>()
{
    return {
        { "pos",    Type::vec3f, (void*)0                        },
        { "uv",     Type::vec2f, (void*)offsetof(shaderData::Vertex, uv)     },
        { "normal", Type::vec3f, (void*)offsetof(shaderData::Vertex, normal) }
    };
}

template<>
inline gfx::StructLayout gfx::getLayout<shaderData::Material>()
{
    return {
        { "ambiant",       Type::vec3f,  (void*)offsetof(shaderData::Material, ambiant)       },
        { "diffuse",       Type::vec3f,  (void*)offsetof(shaderData::Material, diffuse)       },
        { "specular",      Type::vec3f,  (void*)offsetof(shaderData::Material, specular)      },
        { "emissive",      Type::vec3f,  (void*)offsetof(shaderData::Material, emissive)      },
        { "shininess",     Type::Float,  (void*)offsetof(shaderData::Material, shininess)     },
        { "useDiffuseMap", Type::Uint32, (void*)offsetof(shaderData::Material, useDiffuseMap) }
    };
}

template<>
inline gfx::StructLayout gfx::getLayout<shaderData::PointLight>()
{
    return {
        { "position",          Type::vec3f, (void*)offsetof(shaderData::PointLight, position)          },
        { "color",             Type::vec3f, (void*)offsetof(shaderData::PointLight, color)             },
        { "ambiantIntensity",  Type::Float, (void*)offsetof(shaderData::PointLight, ambiantIntensity)  },
        { "diffuseIntensity",  Type::Float, (void*)offsetof(shaderData::PointLight, diffuseIntensity)  },
        { "specularIntensity", Type::Float, (void*)offsetof(shaderData::PointLight, specularIntensity) }
    };
}
#endif

#endif // SHADERDATAS_HPP