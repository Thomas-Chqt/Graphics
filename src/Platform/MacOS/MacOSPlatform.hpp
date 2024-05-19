/*
 * ---------------------------------------------------
 * MacOSPlatform.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/18 14:33:59
 * ---------------------------------------------------
 */

#ifndef MACOSPLATFORM_HPP
# define MACOSPLATFORM_HPP

#include "Graphics/Platform.hpp"

#include "UtilsCPP/Func.hpp"
#include "Graphics/Event.hpp"

#ifdef OBJCPP
    #import <AppKit/AppKit.h>
    #include "Platform/MacOS/ApplicationDelegate.h"
#else
    class NSApplication;
    class ApplicationDelegate;
#endif // OBJCPP

namespace gfx
{

class MacOSPlatform : public Platform
{
    friend void Platform::init();

public:
    MacOSPlatform(const MacOSPlatform&) = delete;
    MacOSPlatform(MacOSPlatform&&)      = delete;

    utils::SharedPtr<Window> newWindow(int w, int h) override;
    void pollEvents() override;
    inline void setEventCallBack(const utils::Func<void(Event&)>& fn) override { m_nextEventCallback = fn; }

    ~MacOSPlatform() override;

private:
    MacOSPlatform();

    void eventCallBack(Event& e);

    NSApplication* m_nsApplication;
    ApplicationDelegate* m_applicationDelegate;
    utils::Func<void(Event&)> m_nextEventCallback;

public:
    MacOSPlatform& operator = (const MacOSPlatform&) = delete;
    MacOSPlatform& operator = (MacOSPlatform&&)      = delete;
};

}

#endif // MACOSPLATFORM_HPP