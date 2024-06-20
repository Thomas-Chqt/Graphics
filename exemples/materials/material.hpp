/*
 * ---------------------------------------------------
 * material.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/20 18:40:47
 * ---------------------------------------------------
 */

#ifndef MATERIAL_HPP
# define MATERIAL_HPP

#include "Math/Vector.hpp"

class material
{
public:
    material();
    

    ~material() = default;

private:
    math::rgba m_baseColor = WHITE;
    float m_specularIntensity = 1;
    float m_specularPower = 1;

};

#endif // MATERIAL_HPP