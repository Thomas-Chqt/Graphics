/*
 * ---------------------------------------------------
 * MetalDrawable.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/23 12:45:44
 * ---------------------------------------------------
 */

#ifndef METALDRAWABLE_HPP
#define METALDRAWABLE_HPP

#include "Graphics/Drawable.hpp"
#include "Metal/MetalTexture.hpp"

#include <memory>

#if !defined(__OBJC__)
#error this file can only by used in objective c
#endif

namespace gfx
{

class MetalDevice;

class MetalDrawable : public Drawable
{
public:
    MetalDrawable() = delete;
    MetalDrawable(const MetalDrawable&) = delete;
    MetalDrawable(MetalDrawable&&) = delete;

    MetalDrawable(const Texture::Descriptor&);

    std::shared_ptr<Texture> texture() const override;

    inline id<CAMetalDrawable> mtlDrawable() const { return m_mtlDrawable; }
    void setMtlDrawable(const id<CAMetalDrawable>& d);

    ~MetalDrawable() override = default;

private:
    id<CAMetalDrawable> m_mtlDrawable = nil;
    std::shared_ptr<MetalTexture> m_texture = nullptr;

public:
    MetalDrawable& operator=(const MetalDrawable&) = delete;
    MetalDrawable& operator=(MetalDrawable&&) = delete;
};

} // namespace gfx

#endif // METALDRAWABLE_HPP
