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

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T>
    using id = T*;
    class CAMetalDrawable;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
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

    MetalDrawable(const MetalDevice*, id<CAMetalDrawable>);

    ext::shared_ptr<Texture> texture() const override;

    const id<CAMetalDrawable>& mtlDrawable() const { return m_mtlDrawable; }

    ~MetalDrawable() override;

private:
    const MetalDevice* m_device;
    id<CAMetalDrawable> m_mtlDrawable;

public:
    MetalDrawable& operator=(const MetalDrawable&) = delete;
    MetalDrawable& operator=(MetalDrawable&&) = delete;
};

} // namespace gfx

#endif // METALDRAWABLE_HPP
