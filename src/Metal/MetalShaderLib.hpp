/*
 * ---------------------------------------------------
 * MetalShaderLib.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/26 10:50:10
 * ---------------------------------------------------
 */

#ifndef METALSHADERLIB_HPP
#define METALSHADERLIB_HPP

#include "Graphics/ShaderLib.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;
    class MTLLibrary;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <filesystem>
    namespace ext = std;
#endif

namespace gfx
{

class MetalDevice;

class MetalShaderLib : public ShaderLib
{
public:
    MetalShaderLib() = delete;
    MetalShaderLib(const MetalShaderLib&) = delete;
    MetalShaderLib(MetalShaderLib&&) = delete;

    MetalShaderLib(const MetalDevice& device, const ext::filesystem::path& filepath);

    ~MetalShaderLib();

private:
    id<MTLLibrary> m_mtlLibrary;

public:
    MetalShaderLib& operator=(const MetalShaderLib&) = delete;
    MetalShaderLib& operator=(MetalShaderLib&&) = delete;
};

} // namespace gfx

#endif // METALSHADERLIB_HPP
