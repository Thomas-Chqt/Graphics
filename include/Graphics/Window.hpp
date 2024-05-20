/*
 * ---------------------------------------------------
 * Window.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/14 17:09:45
 * ---------------------------------------------------
 */

#ifndef WINDOW_HPP
# define WINDOW_HPP

#include "UtilsCPP/Func.hpp"
#include "Event.hpp"

#ifdef OBJCPP
    #import <QuartzCore/CAMetalLayer.h>
#else
    class CAMetalLayer;
#endif // OBJCPP

namespace gfx
{

class Window
{
public:
    Window(const Window&) = delete;
    Window(Window&&)      = delete;

    virtual void setEventCallBack(const utils::Func<void(Event&)>&) = 0;

#ifdef USING_OPENGL
    virtual void useOpenGL() = 0;
    virtual void makeOpenGlContextCurrent() = 0;
    virtual void openGLSwapBuffer() = 0;
#endif
#ifdef USING_METAL
    virtual void useMetal() = 0;
    virtual CAMetalLayer* getMetalLayer() = 0;
#endif

    virtual ~Window() = default;

protected:
    Window() = default;

public:
    Window operator = (const Window&) = delete;
    Window operator = (Window&&)      = delete;
};

}

#endif // WINDOW_HPP