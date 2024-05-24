/*
 * ---------------------------------------------------
 * MetalTexture.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 16:36:59
 * ---------------------------------------------------
 */

#ifndef METALTEXTURE_HPP
# define METALTEXTURE_HPP

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = void*;

    class MTLDevice;
    class MTLTextureDescriptor;
    class MTLTexture;
#endif // OBJCPP


namespace gfx
{

class MetalTexture : public Texture
{
private:
    friend utils::SharedPtr<Texture> MetalGraphicAPI::newTexture(utils::uint32 width, utils::uint32 height);

public:
    MetalTexture()                    = delete;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&)      = delete;

    utils::uint32 width() override;
    utils::uint32 height() override;

    void replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes) override;

    inline id<MTLTexture> mtlTexture() { return m_mtlTexture; } 

    ~MetalTexture() override;

private:
    MetalTexture(id<MTLDevice>, MTLTextureDescriptor*);

    id<MTLTexture> m_mtlTexture = nullptr;

public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&)      = delete;

};

}

#endif // METALTEXTURE_HPP