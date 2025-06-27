/*
 * ---------------------------------------------------
 * ShaderFunction.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 12:20:21
 * ---------------------------------------------------
 */

#ifndef SHADERFUNCTION_HPP
#define SHADERFUNCTION_HPP

namespace gfx
{

class ShaderFunction
{
public:
    ShaderFunction(const ShaderFunction&) = delete;
    ShaderFunction(ShaderFunction&&) = default;

    virtual ~ShaderFunction() = default;

protected:
    ShaderFunction() = default;

public:
    ShaderFunction& operator=(const ShaderFunction&) = delete;
    ShaderFunction& operator=(ShaderFunction&&) = default;
};

}

#endif // SHADERFUNCTION_HPP
