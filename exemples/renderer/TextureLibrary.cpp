/*
 * ---------------------------------------------------
 * TextureLibrary.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 16:29:20
 * ---------------------------------------------------
 */

#include "TextureLibrary.hpp"

#include "UtilsCPP/RuntimeError.hpp"
#include "stb_image.h"

utils::UniquePtr<TextureLibrary> TextureLibrary::s_instance;

const utils::SharedPtr<gfx::Texture>& TextureLibrary::textureFromFile(const utils::String& filePath)
{
    if (m_textures.contain(filePath) == false)
    {
        int width;
        int height;
        stbi_uc* imgBytes = stbi_load(filePath, &width, &height, nullptr, STBI_rgb_alpha);
        if (imgBytes == nullptr)
            throw utils::RuntimeError("fail to read texture at path: " + filePath);

        gfx::Texture::Descriptor textureDescriptor;
        textureDescriptor.width = width;
        textureDescriptor.height = height;

        utils::SharedPtr<gfx::Texture> texture = m_api->newTexture(textureDescriptor);
        texture->setBytes(imgBytes);

        stbi_image_free(imgBytes);

        m_textures.insert(filePath, texture);
    }
    return m_textures[filePath];
}

TextureLibrary::TextureLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_api(api)
{
}