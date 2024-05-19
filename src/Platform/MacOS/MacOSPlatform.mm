/*
 * ---------------------------------------------------
 * MacOSPlatform.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/15 14:50:39
 * ---------------------------------------------------
 */

#include "UtilsCPP/Func.hpp"
#define OBJCPP

#include "Platform/MacOS/MacOSPlatform.hpp"

#include "Logger/Logger.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "Window/MacOS/MacOSWindow.hpp"

using utils::SharedPtr;
using utils::UniquePtr;
using utils::Func;

namespace gfx
{

UniquePtr<Platform> Platform::s_shared;

void Platform::init()
{
    s_shared = UniquePtr<Platform>(new MacOSPlatform());
    logDebug << "MacOSPlatform initialized" << std::endl;
}

void Platform::terminate()
{
    s_shared.clear();
    logDebug << "MacOSPlatform terminated" << std::endl;
}

SharedPtr<Window> MacOSPlatform::newWindow(int w, int h)
{
    return SharedPtr<Window>(new MacOSWindow(w, h, Func<void(Event&)>(*this, &MacOSPlatform::eventCallBack)));
}

void MacOSPlatform::pollEvents() { @autoreleasepool
{
    while(true)
    {
        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event == nil)
            break;
        [NSApp sendEvent:event];
    }
}}

MacOSPlatform::~MacOSPlatform() { @autoreleasepool
{
    [m_applicationDelegate release];
}}

MacOSPlatform::MacOSPlatform() { @autoreleasepool
{
    m_nsApplication = [NSApplication sharedApplication];
    m_applicationDelegate = [[ApplicationDelegate alloc] initWithCallBack:Func<void(Event&)>(*this, &MacOSPlatform::eventCallBack)];

    m_nsApplication.delegate = m_applicationDelegate;
    [m_nsApplication run];
    m_nsApplication.activationPolicy = NSApplicationActivationPolicyRegular;
}}

void MacOSPlatform::eventCallBack(Event& e)
{
    if (m_nextEventCallback)
        m_nextEventCallback(e);
}

}