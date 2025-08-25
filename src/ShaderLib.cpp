/*
 * ---------------------------------------------------
 * ShaderLib.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/26 09:33:14
 * ---------------------------------------------------
 */

#include "Graphics/ShaderLib.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <filesystem>
    #include <fstream>
    #include <string>
    #include <vector>
    #include <cstdint>
    #include <stdexcept>
    #include <array>
    #include <bit>
    namespace ext = std;
#endif

namespace fs = ext::filesystem;

namespace gfx
{

ShaderLib::ShaderLib(const fs::path& filepath)
{
    ext::ifstream file(filepath, std::ios::binary);
    if (file.good() == false)
        throw ext::runtime_error("cannot open the file");

    std::array<char, 19> magic = {};
    file.read(magic.data(), 18);

    if (ext::strcmp(magic.data(), "GFX_SHADER_PACKAGE") != 0)
        throw ext::runtime_error("file format invalid");

    std::array<char, sizeof(uint32_t)> shadercount = {};
    file.read(shadercount.data(), shadercount.size());

    for (uint32_t i = 0; i < ext::bit_cast<uint32_t>(shadercount); ++i)
    {
        std::array<char, sizeof(uint32_t)> targetNameLength = {};
        file.read(targetNameLength.data(), targetNameLength.size());

        std::string targetName(ext::bit_cast<uint32_t>(targetNameLength), '\0');
        file.read(targetName.data(), static_cast<long>(targetName.size()));

        std::array<char, sizeof(uint32_t)> shaderLength = {};
        file.read(shaderLength.data(), shaderLength.size());

        if (targetName == "metal")
        {
            m_metalBytes.resize(ext::bit_cast<uint32_t>(shaderLength));
            file.read(ext::bit_cast<char*>(m_metalBytes.data()), static_cast<long>(m_metalBytes.size()));
        }
        else if (targetName == "spirv")
        {
            m_spirvBytes.resize(ext::bit_cast<uint32_t>(shaderLength));
            file.read(ext::bit_cast<char*>(m_spirvBytes.data()), static_cast<long>(m_spirvBytes.size()));
        }
        else
        {
            file.seekg(ext::bit_cast<uint32_t>(shaderLength), std::ios::cur);
        }
    }
}

} // namespace gfx
