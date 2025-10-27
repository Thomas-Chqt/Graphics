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

#include "Metal/MetalShaderFunction.hpp"

namespace gfx
{

class MetalDevice;

class MetalShaderLib : public ShaderLib
{
public:
    MetalShaderLib() = delete;
    MetalShaderLib(const MetalShaderLib&) = delete;
    MetalShaderLib(MetalShaderLib&&) = delete;

    MetalShaderLib(const MetalDevice& device, const std::filesystem::path& filepath);

    MetalShaderFunction& getFunction(const std::string&) override;

    ~MetalShaderLib() override;

private:
    id<MTLLibrary> m_mtlLibrary;

    std::map<std::string, MetalShaderFunction> m_shaderFunctions;

public:
    MetalShaderLib& operator=(const MetalShaderLib&) = delete;
    MetalShaderLib& operator=(MetalShaderLib&&) = delete;
};

} // namespace gfx

#endif // METALSHADERLIB_HPP
