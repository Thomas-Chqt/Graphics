/*
 * ---------------------------------------------------
 * Light.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/20 18:55:00
 * ---------------------------------------------------
 */

#ifndef LIGHT_HPP
# define LIGHT_HPP

#include "Math/Vector.hpp"

class Light
{
public:
    Light();
    

    ~Light();

private:
    math::rgb m_color = {1, 1, 1};
    float ambiantIntensity = 0.5;
};

#endif // LIGHT_HPP