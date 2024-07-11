/*
 * ---------------------------------------------------
 * MetalShader.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 14:13:49
 * ---------------------------------------------------
 */

#ifndef METALSHADER_HPP
# define METALSHADER_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Shader.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;

    class MTLDevice;
    class MTLFunction;
#endif // __OBJC__


namespace gfx
{

class MetalShader : public Shader
{
public:
    MetalShader()                   = delete;
    MetalShader(const MetalShader&) = delete;
    MetalShader(MetalShader&&)      = delete;
    
    MetalShader(const id<MTLDevice>&, const MetalShaderDescriptor&);

    inline ShaderType type() override { return m_shaderType; }

    inline const id<MTLFunction>& mtlFunction() { return m_mtlFunction; }

    ~MetalShader() override;

private:
    id<MTLFunction> m_mtlFunction;
    const ShaderType m_shaderType;

public:
    MetalShader& operator = (const MetalShader&) = delete;
    MetalShader& operator = (MetalShader&&)      = delete;
};

}

#endif // METALSHADER_HPP