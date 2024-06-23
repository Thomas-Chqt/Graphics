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

#ifndef __METAL_VERSION__
    #include "Graphics/StructLayout.hpp"
#endif

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
};

#ifndef __METAL_VERSION__
    template<>
    inline gfx::StructLayout gfx::getLayout<Vertex>()
    {
        return {
            { "pos", Type::vec3f, (void*)0 },
            { "uv", Type::vec2f, (void*)offsetof(Vertex, uv) },
            { "normal", Type::vec3f, (void*)offsetof(Vertex, normal) }
        };
    }
#endif

struct Material
{
    math::rgb ambiant;
    math::rgb diffuse;
    math::rgb specular;
    math::rgb emissive;
    float shininess;
};

#ifndef __METAL_VERSION__
    template<>
    inline gfx::StructLayout gfx::getLayout<Material>()
    {
        return {
            { "ambiant",   Type::vec3f, (void*)offsetof(Material, ambiant)   },
            { "diffuse",   Type::vec3f, (void*)offsetof(Material, diffuse)   },
            { "specular",  Type::vec3f, (void*)offsetof(Material, specular)  },
            { "emissive",  Type::vec3f, (void*)offsetof(Material, emissive)  },
            { "shininess", Type::Float, (void*)offsetof(Material, shininess) }
        };
    }
#endif

struct PointLight
{
    math::vec3f position;
    math::rgb color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

#ifndef __METAL_VERSION__
    template<>
    inline gfx::StructLayout gfx::getLayout<PointLight>()
    {
        return {
            { "position",          Type::vec3f, (void*)offsetof(PointLight, position)          },
            { "color",             Type::vec3f, (void*)offsetof(PointLight, color)             },
            { "ambiantIntensity",  Type::Float, (void*)offsetof(PointLight, ambiantIntensity)  },
            { "diffuseIntensity",  Type::Float, (void*)offsetof(PointLight, diffuseIntensity)  },
            { "specularIntensity", Type::Float, (void*)offsetof(PointLight, specularIntensity) }
        };
    }
#endif

#endif // SHADERDATAS_HPP