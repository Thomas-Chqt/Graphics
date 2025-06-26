/*
 * ---------------------------------------------------
 * ShaderLib.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/25 17:28:24
 * ---------------------------------------------------
 */

#ifndef SHADERLIB_HPP
#define SHADERLIB_HPP

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <filesystem>
    #include <cstddef>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class ShaderLib
{
public:
    ShaderLib() = delete;
    ShaderLib(const ShaderLib&) = delete;
    ShaderLib(ShaderLib&&) = delete;

    virtual ~ShaderLib() = default;

protected:
    ShaderLib(const ext::filesystem::path&);

    ext::vector<ext::byte> m_metalBytes;
    ext::vector<ext::byte> m_spirvBytes;

public:
    ShaderLib& operator=(const ShaderLib&) = delete;
    ShaderLib& operator=(ShaderLib&&) = delete;
};

} // namespace gfx

#endif // SHADERLIB_HPP
