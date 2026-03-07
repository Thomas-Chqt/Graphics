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
#include "Graphics/Sampler.hpp"

#include <cstdint>
#include <memory>
#include <optional>

#if defined (GFX_IMGUI_ENABLED)
using ImTextureID = uint64_t;
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
        auto operator<=>(const Descriptor&) const = default;
    };

public:
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;

    virtual TextureType type() const = 0;
    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual PixelFormat pixelFormat() const = 0;
    virtual TextureUsages usages() const = 0;
    virtual ResourceStorageMode storageMode() const = 0;

#if defined (GFX_IMGUI_ENABLED)
    virtual void initImTextureId(const std::shared_ptr<Sampler>&) = 0;
    virtual std::optional<ImTextureID> imTextureId() const = 0;
#endif

    virtual ~Texture() = default;

protected:
    Texture() = default;

public:
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;
};

} // namespace gfx

#endif // TEXTURE_HPP
