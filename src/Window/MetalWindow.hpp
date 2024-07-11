/*
 * ---------------------------------------------------
 * MetalWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 09:17:38
 * ---------------------------------------------------
 */

#ifndef METALWINDOW_HPP
# define METALWINDOW_HPP

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Window/Window_internal.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = T*;

    class MTLDevice;
    class CAMetalDrawable;
#endif // __OBJC__


namespace gfx
{

class MetalGraphicAPI;

class MetalWindow : virtual public Window_internal
{
public:
    MetalWindow(const MetalWindow&) = delete;
    MetalWindow(MetalWindow&&)      = delete;

    virtual void setGraphicAPI(MetalGraphicAPI*) = 0;

    virtual void makeCurrentDrawables() = 0;
    virtual const id<CAMetalDrawable>& currentDrawable() = 0;
    virtual const MetalTexture& currentDepthTexture() = 0;
    virtual void clearCurrentDrawables() = 0;

    ~MetalWindow() override = default;

protected:
    MetalWindow() = default;

public:
    MetalWindow& operator = (const MetalWindow&) = delete;
    MetalWindow& operator = (MetalWindow&&)      = delete;
};

}

#endif // METALWINDOW_HPP