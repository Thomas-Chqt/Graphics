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
#endif // __OBJC__


namespace gfx
{

class MetalTexture : public Texture
{
public:
    MetalTexture()                    = default;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&) noexcept;

    MetalTexture(id<MTLDevice>, const Texture::Descriptor&);

    utils::uint32 width() override;
    utils::uint32 height() override;

    inline void replaceRegion(const Region& region, const void* bytes) override { replaceRegion(region, 0, bytes); }
    void replaceRegion(const Region&, utils::uint32 slice, const void* bytes) override;

    inline id<MTLTexture> mtlTexture() const { return m_mtlTexture; } //NOLINT(modernize-use-nodiscard)

    #ifdef GFX_BUILD_IMGUI
    inline void* imguiTextureId() const override { return (void*)mtlTexture(); }
    inline ImVec2 imguiUV0() const override { return ImVec2(0, 0); }
    inline ImVec2 imguiUV1() const override { return ImVec2(1, 1); }
    #endif


    ~MetalTexture() override;

private:
    id<MTLTexture> m_mtlTexture = nullptr;

public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&) noexcept;

};

}

#endif // METALTEXTURE_HPP
