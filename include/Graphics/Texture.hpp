/*
 * ---------------------------------------------------
 * Texture.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:06:02
 * ---------------------------------------------------
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
#else
    #include <cstdint>
#endif

namespace gfx
{

class Texture
{
public:
    struct Descriptor
    {
        TextureType type = TextureType::texture2d;
        uint32_t width = 0, height = 0;
        PixelFormat pixelFormat = PixelFormat::RGBA8Unorm;
        TextureUsages usages = TextureUsage::shaderRead;
        ResourceStorageMode storageMode = ResourceStorageMode::deviceLocal;
    };

public:
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual PixelFormat pixelFormat() const = 0;
    virtual TextureUsages usages() const = 0;
    virtual ResourceStorageMode storageMode() const = 0;

    virtual ~Texture() = default;

protected:
    Texture() = default;

public:
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;
};

} // namespace gfx

#endif // TEXTURE_HPP
