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
    #include "Graphics/VertexBuffer.hpp"
    #include "UtilsCPP/Array.hpp"
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
    math::rgb color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

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
    inline utils::Array<gfx::VertexBuffer::LayoutElement> gfx::VertexBuffer::getLayout<Vertex>()
    {
        return {
            { 3, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, pos)    },
            { 2, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, uv)     },
            { 3, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal) }
        };
    }
#endif

#endif // SHADERDATAS_HPP