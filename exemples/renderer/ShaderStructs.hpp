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

#include "Math/Vector.hpp"

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

namespace baseTexture
{
    struct Material
    {
        math::rgb specularColor;
        math::rgb emissiveColor;
        float     shininess;
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

#ifndef __METAL_VERSION__
} // namespace shaderStruct
#endif

#endif // SHADERSTRUCTS_HPP