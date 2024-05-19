/*
 * ---------------------------------------------------
 * MacOSWindow.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/30 22:27:26
 * ---------------------------------------------------
 */

#define OBJCPP

#include "Window/MacOS/MacOSWindow.hpp"
#include "UtilsCPP/Func.hpp"
#include "Logger/Logger.hpp"
#include "Window/MacOS/Views/EmptyView.h"

using utils::Func;

namespace gfx
{

MacOSWindow::MacOSWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback)
    : m_nextEventCallback(defaultCallback) { @autoreleasepool
{
    m_windowDelegate = [[WindowDelegate alloc] initWithGFXWindow:*this
                                                     andCallBack:Func<void(Event&)>(*this, &MacOSWindow::eventCallBack)];

    m_nsWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(900, 800, w, h)
                                             styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                               backing:NSBackingStoreBuffered
                                                 defer:YES];
    m_nsWindow.delegate = m_windowDelegate;

    m_nsWindow.acceptsMouseMovedEvents = YES;
    
    [m_nsWindow makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];

    logDebug << "MacOSWindow created" << std::endl;

    m_contentView = [[[EmptyView alloc] initWithGFXWindow:*this
                                              andCallBack:Func<void(Event&)>(*this, &MacOSWindow::eventCallBack)] autorelease];

    m_nsWindow.contentView = (NSView*)m_contentView;
    [m_nsWindow makeFirstResponder:(NSView*)m_contentView];

    logDebug << "Content view set to Empty view" << std::endl;
}}

MacOSWindow::~MacOSWindow() { @autoreleasepool
{
    [m_nsWindow close];
    [m_windowDelegate release];
    logDebug << "MacOSWindow deleted" << std::endl;
}}

}