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

#include "Graphics/ShaderFunction.hpp"

#include <filesystem>
#include <cstddef>
#include <vector>
#include <string>

namespace gfx
{

class ShaderLib
{
public:
    ShaderLib() = delete;
    ShaderLib(const ShaderLib&) = delete;
    ShaderLib(ShaderLib&&) = delete;

    virtual ShaderFunction& getFunction(const std::string&) = 0;

    virtual ~ShaderLib() = default;

protected:
    ShaderLib(const std::filesystem::path&);

    std::vector<std::byte> m_metalBytes;
    std::vector<std::byte> m_spirvBytes;

public:
    ShaderLib& operator=(const ShaderLib&) = delete;
    ShaderLib& operator=(ShaderLib&&) = delete;
};

} // namespace gfx

#endif // SHADERLIB_HPP
