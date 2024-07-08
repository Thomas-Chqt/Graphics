/*
 * ---------------------------------------------------
 * shaderShared.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 18:19:49
 * ---------------------------------------------------
 */

#ifndef SHADERSHARED_HPP
# define SHADERSHARED_HPP

#ifndef __METAL_VERSION__
    #include "UtilsCPP/StructLayout.hpp"
#endif // __METAL_VERSION__

#include "Math/Vector.hpp"

#ifndef __METAL_VERSION__
namespace shaderShared {
#endif // __METAL_VERSION__

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
};

struct DirectionalLight
{
    math::vec3f direction;
    float intensity;
    math::rgb color;
};

struct Material
{
    math::rgba baseColor;
};

#ifndef __METAL_VERSION__
} // namespace shaderShared
#endif // __METAL_VERSION__

#ifndef __METAL_VERSION__

template<>
inline utils::StructLayout utils::getLayout<shaderShared::Vertex>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(math::vec3f, 1, "pos"),
        STRUCT_LAYOUT_ELEMENT(math::vec2f, 1, "uv"),
        STRUCT_LAYOUT_ELEMENT(math::vec3f, 1, "normal")
    };
}

template<>
inline utils::StructLayout utils::getLayout<shaderShared::DirectionalLight>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(math::vec3f, 1, "direction"),
        STRUCT_LAYOUT_ELEMENT(float,       1, "intensity"),
        STRUCT_LAYOUT_ELEMENT(math::rgb,   1, "color")
    };
}

template<>
inline utils::StructLayout utils::getLayout<shaderShared::Material>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(math::rgba, 1, "baseColor")
    };
}
#endif // __METAL_VERSION__

#endif // SHADERSHARED_HPP