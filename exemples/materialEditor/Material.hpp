/*
 * ---------------------------------------------------
 * Material.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 17:51:38
 * ---------------------------------------------------
 */

#ifndef MATERIAL_HPP
# define MATERIAL_HPP

#include "Math/Vector.hpp"
#include "shaderShared.hpp"

class Material
{
public:
    Material()                = default;
    Material(const Material&) = default;
    Material(Material&&)      = default;

    shaderShared::Material getShaderShared() const;
    
    ~Material() = default;

private:
    math::rgba baseColor = WHITE;

public:
    Material& operator = (const Material&) = default;
    Material& operator = (Material&&)      = default;
};

#endif // MATERIAL_HPP