/*
 * ---------------------------------------------------
 * DirectionalLight.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 16:19:09
 * ---------------------------------------------------
 */

#ifndef DIRECTIONALLIGHT_HPP
# define DIRECTIONALLIGHT_HPP

#include "Math/Vector.hpp"

struct DirectionalLight
{
    math::rgb color         = WHITE3;
    float ambiantIntensity  = 0.25;
    float diffuseIntensity  = 0.5;
    float specularIntensity = 0.5;
    math::vec3f direction   = { 1.0F, 0.0F, 0.0F };
};

#endif // DIRECTIONALLIGHT_HPP