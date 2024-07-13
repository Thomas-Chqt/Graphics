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

#include "Math/Vector.hpp"

struct Material
{
    math::vec4f baseColor = WHITE;
};

#endif // MATERIAL_HPP