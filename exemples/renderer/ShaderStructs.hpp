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
    #include "UtilsCPP/StructLayout.hpp"
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
inline utils::StructLayout utils::getLayout<shaderStruct::PointLight>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(math::vec3f, 1, "position"),
        STRUCT_LAYOUT_ELEMENT(  math::rgb, 1, "color"),
        STRUCT_LAYOUT_ELEMENT(      float, 1, "ambiantIntensity"),
        STRUCT_LAYOUT_ELEMENT(      float, 1, "diffuseIntensity"),
        STRUCT_LAYOUT_ELEMENT(      float, 1, "specularIntensity")
    };
}

template<>
inline utils::StructLayout utils::getLayout<shaderStruct::universal::Material>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(    math::rgb, 1, "ambientColor"),
        STRUCT_LAYOUT_ELEMENT(utils::uint32, 1, "useAmbientTexture"),
        STRUCT_LAYOUT_ELEMENT(    math::rgb, 1, "diffuseColor"),
        STRUCT_LAYOUT_ELEMENT(utils::uint32, 1, "useDiffuseTexture"),
        STRUCT_LAYOUT_ELEMENT(    math::rgb, 1, "specularColor"),
        STRUCT_LAYOUT_ELEMENT(utils::uint32, 1, "useSpecularTexture"),
        STRUCT_LAYOUT_ELEMENT(    math::rgb, 1, "emissiveColor"),
        STRUCT_LAYOUT_ELEMENT(utils::uint32, 1, "useEmissiveTexture"),
        STRUCT_LAYOUT_ELEMENT(        float, 1, "shininess"),
        STRUCT_LAYOUT_ELEMENT(utils::uint32, 1, "useShininessTexture")
   };
}

template<>
inline utils::StructLayout utils::getLayout<shaderStruct::baseColor::Material>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(math::rgb, 1, "baseColor"),
        STRUCT_LAYOUT_ELEMENT(math::rgb, 1, "specularColor"),
        STRUCT_LAYOUT_ELEMENT(math::rgb, 1, "emissiveColor"),
        STRUCT_LAYOUT_ELEMENT(    float, 1, "shininess")
    };
}

template<>
inline utils::StructLayout utils::getLayout<shaderStruct::baseTexture::Material>()
{
    return {
        STRUCT_LAYOUT_ELEMENT(math::rgb, 1, "specularColor"),
        STRUCT_LAYOUT_ELEMENT(math::rgb, 1, "emissiveColor"),
        STRUCT_LAYOUT_ELEMENT(    float, 1, "shininess")
    };
}
#endif

#endif // SHADERSTRUCTS_HPP