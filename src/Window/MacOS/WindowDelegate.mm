/*
 * ---------------------------------------------------
 * WindowDelegate.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/30 23:14:10
 * ---------------------------------------------------
 */

#include "Window/MacOS/WindowDelegate.h"
#include "Graphics/Event.hpp"

using gfx::WindowResizeEvent;
using gfx::WindowRequestCloseEvent;

@implementation WindowDelegate
{
    gfx::Window* m_gfxWindow;
    utils::Func<void(gfx::Event&)> m_eventCallBackFunc;
}

- (instancetype)initWithGFXWindow:(gfx::Window&)win andCallBack:(const utils::Func<void(gfx::Event&)>&)callBack
{
    [super init];
    m_gfxWindow = &win;
    m_eventCallBackFunc = callBack;
    return self;
}

- (void)windowDidResize:(NSNotification*)notification
{
    assert(m_eventCallBackFunc == true);

    NSWindow* window = (NSWindow *)[notification object];
    NSRect frame = window.frame;
    WindowResizeEvent windowResizeEvent(*m_gfxWindow, frame.size.width, frame.size.height);
    m_eventCallBackFunc(windowResizeEvent);
}

- (BOOL)windowShouldClose:(NSWindow*)sender
{
    assert(m_eventCallBackFunc == true);

    WindowRequestCloseEvent windowRequestCloseEvent(*m_gfxWindow);
    m_eventCallBackFunc(windowRequestCloseEvent);
    return NO;
}

@end