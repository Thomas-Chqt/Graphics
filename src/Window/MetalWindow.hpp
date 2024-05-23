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

#include "Window/Window_internal.hpp"

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
#else
    class CAMetalLayer;
#endif // OBJCPP

namespace gfx
{

class MetalWindow : public Window_internal
{
public:
    MetalWindow(const MetalWindow&) = delete;
    MetalWindow(MetalWindow&&)      = delete;

    virtual CAMetalLayer* metalLayer() = 0;    

    virtual ~MetalWindow() = default;

protected:
    MetalWindow() = default;

public:
    MetalWindow& operator = (const MetalWindow&) = delete;
    MetalWindow& operator = (MetalWindow&&)      = delete;
};

}

#endif // METALWINDOW_HPP