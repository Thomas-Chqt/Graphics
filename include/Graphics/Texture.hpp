/*
 * ---------------------------------------------------
 * Texture.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 14:01:14
 * ---------------------------------------------------
 */

#ifndef TEXTURE_HPP
# define TEXTURE_HPP

#include "Enums.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/Macros.hpp"
#ifdef GFX_BUILD_IMGUI
    #include "imgui/imgui.h"
#endif

namespace gfx
{

class Texture
{
public:
    enum class Type  { texture2d, textureCube };
    enum class Usage { ShaderRead, RenderTarget, ShaderReadAndRenderTarget };

    struct Descriptor
    {
        static inline Descriptor texture2dDescriptor(utils::uint32 width, utils::uint32 height)
        {
            Descriptor descriptor;
            descriptor.type        = Texture::Type::texture2d;
            descriptor.width       = width;
            descriptor.height      = height;
            descriptor.pixelFormat = PixelFormat::RGBA;
            descriptor.storageMode = StorageMode::Managed;
            descriptor.usage       = Texture::Usage::ShaderRead;
            return descriptor;
        }
        
        static inline Descriptor depthTextureDescriptor(utils::uint32 width, utils::uint32 height)
        {
            Descriptor descriptor;
            descriptor.type        = Texture::Type::texture2d;
            descriptor.width       = width;
            descriptor.height      = height;
            descriptor.pixelFormat = PixelFormat::Depth32;
            descriptor.storageMode = StorageMode::Private;
            descriptor.usage       = Texture::Usage::RenderTarget;
            return descriptor;
        }

        static inline Descriptor textureCubeDescriptor(utils::uint32 size)
        {
            Descriptor descriptor;
            descriptor.type        = Texture::Type::textureCube;
            descriptor.width       = size;
            descriptor.height      = size;
            descriptor.pixelFormat = PixelFormat::RGBA;
            descriptor.storageMode = StorageMode::Managed;
            descriptor.usage       = Texture::Usage::ShaderRead;
            return descriptor;
        }

        Texture::Type type = Texture::Type::texture2d;
        utils::uint32 width = 0;
        utils::uint32 height = 0;
        gfx::PixelFormat pixelFormat = PixelFormat::RGBA;
        StorageMode storageMode = StorageMode::Managed;
        Texture::Usage usage = Texture::Usage::ShaderRead;
    };

    struct Region
    {
        utils::uint32 offsetX;
        utils::uint32 offsetY;
        utils::uint32 width;
        utils::uint32 height;
    };

public:
    Texture(const Texture&) = delete;
    Texture(Texture&&)      = delete;

    virtual utils::uint32 width() = 0;
    virtual utils::uint32 height() = 0;

    virtual void replaceRegion(const Region&, const void* bytes) = 0;
    virtual void replaceRegion(const Region&, utils::uint32 slice, const void* bytes) = 0;

    inline void replaceContent(const void* data) { replaceRegion({0, 0, width(), height()}, data); }
    inline void replaceSliceContent(utils::uint32 slice, const void* data) { replaceRegion({0, 0, width(), height()}, slice, data); }

    #ifdef GFX_BUILD_IMGUI
    virtual void* imguiTextureId() const = 0;
    virtual ImVec2 imguiUV0() const = 0;
    virtual ImVec2 imguiUV1() const = 0;
    #endif

    virtual ~Texture() = default;

protected:
    Texture() = default;

public:
    Texture& operator = (const Texture&) = delete;
    Texture& operator = (Texture&&)      = delete;
};

}

#endif // TEXTURE_HPP
