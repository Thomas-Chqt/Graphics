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

#include "Graphics/Texture.hpp"
#include "UtilsCPP/Types.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;

    class MTLDevice;
    class MTLTextureDescriptor;
    class MTLTexture;
#endif // OBJCPP


namespace gfx
{

class MetalTexture : public Texture
{
public:
    MetalTexture()                    = default;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&);

    MetalTexture(id<MTLDevice>, const Texture::Descriptor&);

    utils::uint32 width() override;
    utils::uint32 height() override;

    void replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes) override;

    inline id<MTLTexture> mtlTexture() const { return m_mtlTexture; } 

    ~MetalTexture() override;

private:
    id<MTLTexture> m_mtlTexture = nullptr;

public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&);

};

}

#endif // METALTEXTURE_HPP
