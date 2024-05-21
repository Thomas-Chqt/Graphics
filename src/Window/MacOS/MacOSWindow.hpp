/*
* ---------------------------------------------------
* MacOSWindow.hpp
*
* Author: Thomas Choquet <thomas.publique@icloud.com>
* Date: 2023/11/14 18:55:52
* ---------------------------------------------------
*/

#ifndef MACOSWINDOW_H
# define MACOSWINDOW_H

#include "Graphics/Window.hpp"

#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Func.hpp"
#include "Graphics/Event.hpp"
#include "Platform/MacOS/MacOSPlatform.hpp"

#ifdef OBJCPP
    #import <AppKit/AppKit.h>
    #import <QuartzCore/CAMetalLayer.h>
    #include "Window/MacOS/WindowDelegate.h"
    #include "Window/MacOS/Views/MacOSWindowView.h"
#else
    template<typename T> using id = void*;

    class NSWindow;
    class WindowDelegate;
    class MacOSWindowView;
    class CAMetalLayer;
#endif

namespace gfx
{

class MacOSWindow : public Window 
{
private:
    friend utils::SharedPtr<Window> MacOSPlatform::newWindow(int, int);

public:
    MacOSWindow()                   = delete;
    MacOSWindow(const MacOSWindow&) = delete;
    MacOSWindow(MacOSWindow&&)      = delete;

    inline void setEventCallBack(const utils::Func<void(Event&)>& fn) override { m_nextEventCallback = fn; }
    
#ifdef USING_OPENGL
    void useOpenGL() override;
    void makeOpenGlContextCurrent() override;
    void openGLSwapBuffer() override;
#endif
#ifdef USING_METAL
    void useMetal() override;
    CAMetalLayer* getMetalLayer() override;
#endif

    ~MacOSWindow() override;

private:
    MacOSWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback);

    inline void eventCallBack(Event& e) { m_nextEventCallback(e); }

    NSWindow* m_nsWindow;
    WindowDelegate* m_windowDelegate;
    id<MacOSWindowView> m_contentView;
    utils::Func<void(Event&)> m_nextEventCallback;

public:
    MacOSWindow& operator = (const MacOSWindow&) = delete;
    MacOSWindow& operator = (MacOSWindow&&)      = delete;

};

}

#endif // MACOSWINDOW_H
