/*
 * ---------------------------------------------------
 * Material.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 15:53:06
 * ---------------------------------------------------
 */

#ifndef MATERIAL_HPP
# define MATERIAL_HPP

#include "Graphics/Texture.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"

struct ColorInput
{
    ColorInput(math::rgb color) : value(color) {}

    math::rgb value;
    utils::SharedPtr<gfx::Texture> texture;
};

struct Material
{
    ColorInput diffuse  = WHITE3;
    ColorInput specular = BLACK3;
    ColorInput emissive = BLACK3;
    float shininess     = 1.0F;
    utils::SharedPtr<gfx::Texture> normalMap;
};

#endif // MATERIAL_HPP