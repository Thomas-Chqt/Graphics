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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <string>
    namespace ext = std;
#endif

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T>
    using id = T*;
    class MTLLibrary;
    class MTLFunction;
#endif // __OBJC__

namespace gfx
{

class MetalShaderFunction : public ShaderFunction
{
public:
    MetalShaderFunction() = delete;
    MetalShaderFunction(const MetalShaderFunction&) = delete;
    MetalShaderFunction(MetalShaderFunction&&);

    MetalShaderFunction(const id<MTLLibrary>&, const ext::string&);

    ~MetalShaderFunction();

private:
    id<MTLFunction> m_mtlFunction;

public:
    MetalShaderFunction& operator=(const MetalShaderFunction&) = delete;
    MetalShaderFunction& operator=(MetalShaderFunction&&);
};

} // namespace gfx

#endif // METALSHADERFUNCTION_HPP
