/*
 * ---------------------------------------------------
 * Shader.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 12:42:29
 * ---------------------------------------------------
 */

#ifndef SHADER_HPP
# define SHADER_HPP

#include "Enums.hpp"
#include "UtilsCPP/String.hpp"

namespace gfx
{

class Shader
{
public:
    struct Descriptor
    {
        ShaderType type;
        utils::String mtlShaderLibPath;
        utils::String mtlFunction;
        utils::String openglCode;
    };

public:
    Shader(const Shader&) = delete;
    Shader(Shader&&)      = delete;

    virtual ShaderType type() = 0;

    virtual ~Shader() = default;

protected:
    Shader() = default;
    
public:
    Shader& operator = (const Shader&) = delete;
    Shader& operator = (Shader&&)      = delete;
};

}

#endif // SHADER_HPP