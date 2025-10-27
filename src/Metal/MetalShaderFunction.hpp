/*
 * ---------------------------------------------------
 * MetalShaderFunction.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 12:25:16
 * ---------------------------------------------------
 */

#ifndef METALSHADERFUNCTION_HPP
#define METALSHADERFUNCTION_HPP

#include "Graphics/ShaderFunction.hpp"

namespace gfx
{

class MetalShaderFunction : public ShaderFunction
{
public:
    MetalShaderFunction() = delete;
    MetalShaderFunction(const MetalShaderFunction&) = delete;
    MetalShaderFunction(MetalShaderFunction&&) noexcept ;

    MetalShaderFunction(const id<MTLLibrary>&, const std::string&);

    inline id<MTLFunction> mtlFunction() { return m_mtlFunction; }

    ~MetalShaderFunction() override;

private:
    id<MTLFunction> m_mtlFunction;

public:
    MetalShaderFunction& operator=(const MetalShaderFunction&) = delete;
    MetalShaderFunction& operator=(MetalShaderFunction&&) noexcept ;
};

} // namespace gfx

#endif // METALSHADERFUNCTION_HPP
