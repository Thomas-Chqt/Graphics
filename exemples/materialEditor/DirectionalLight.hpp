/*
 * ---------------------------------------------------
 * DirectionalLight.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/07 09:20:30
 * ---------------------------------------------------
 */

#ifndef DIRECTIONALLIGHT_HPP
# define DIRECTIONALLIGHT_HPP

#include "Math/Vector.hpp"
#include "shaderShared.hpp"

class DirectionalLight
{
public:
    DirectionalLight()                        = default;
    DirectionalLight(const DirectionalLight&) = default;
    DirectionalLight(DirectionalLight&&)      = default;

    shaderShared::DirectionalLight getShaderShared() const;

    ~DirectionalLight() = default;

private:
    math::vec3f direction = { 0, -1, 0 };
    float intensity       = 1.0f;
    math::rgb color       = WHITE3;

public:
    DirectionalLight& operator = (const DirectionalLight&) = default;
    DirectionalLight& operator = (DirectionalLight&&)      = default;
};

#endif // DIRECTIONALLIGHT_HPP