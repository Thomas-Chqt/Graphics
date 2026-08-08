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

#include <cstdint>

namespace gfx
{

class Texture
{
public:
    struct Descriptor
    {
        TextureType type = TextureType::texture2d;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t mipLevelCount = 1;
        uint32_t arrayLayerCount = 1;
        PixelFormat pixelFormat = PixelFormat::RGBA8_unorm;
        TextureUsages usages = TextureUsage::shaderRead;
        ResourceStorageMode storageMode = ResourceStorageMode::deviceLocal;

        auto operator<=>(const Descriptor&) const = default;
    };

    struct ViewDescriptor
    {
        TextureType type = TextureType::texture2d;
        uint32_t baseMipLevel = 0;
        uint32_t mipLevelCount = 1;
        uint32_t baseArrayLayer = 0;
        uint32_t arrayLayerCount = 1;

        auto operator<=>(const ViewDescriptor&) const = default;
    };

public:
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;

    virtual TextureType type() const = 0;

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;

    virtual uint32_t baseMipLevel() const = 0;
    virtual uint32_t mipLevelCount() const = 0;

    virtual uint32_t baseArrayLayer() const = 0;
    virtual uint32_t arrayLayerCount() const = 0;

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
