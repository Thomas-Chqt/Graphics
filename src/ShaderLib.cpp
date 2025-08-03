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

    char magic[19] = {0};
    file.read(magic, 18);

    if (ext::string(magic) != "GFX_SHADER_PACKAGE")
        throw ext::runtime_error("file format invalid");

    uint32_t shaderCount = 0;
    file.read(reinterpret_cast<char*>(&shaderCount), sizeof(shaderCount));

    for (uint32_t i = 0; i < shaderCount; ++i)
    {
        uint32_t targetLength = 0;
        file.read(reinterpret_cast<char*>(&targetLength), sizeof(targetLength));

        std::string targetName(targetLength, '\0');
        file.read(&targetName[0], targetLength);

        uint32_t shaderLength = 0;
        file.read(reinterpret_cast<char*>(&shaderLength), sizeof(shaderLength));

        if (targetName == "metal")
        {
            m_metalBytes.resize(shaderLength);
            file.read(reinterpret_cast<char*>(m_metalBytes.data()), shaderLength);
        }
        else if (targetName == "spirv")
        {
            m_spirvBytes.resize(shaderLength);
            file.read(reinterpret_cast<char*>(m_spirvBytes.data()), shaderLength);
        }
        else
        {
            file.seekg(shaderLength, std::ios::cur);
        }
    }
}

} // namespace gfx
