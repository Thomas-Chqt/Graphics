/*
 * ---------------------------------------------------
 * MetalView.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/26 17:37:22
 * ---------------------------------------------------
 */

#define OBJCPP

#include "Window/MacOS/Views/MetalView.h"

using gfx::KeyDownEvent;
using gfx::KeyUpEvent;
using gfx::MouseDownEvent;
using gfx::MouseUpEvent;
using gfx::MouseMoveEvent;

@implementation MetalView
{
    gfx::Window* m_gfxWindow;
    utils::Func<void(gfx::Event&)> m_eventCallBackFunc;
}

- (instancetype)initWithGFXWindow:(gfx::Window&)win andCallBack:(const utils::Func<void(gfx::Event&)>&)callBack
{
    [super init];

    m_gfxWindow = &win;
    m_eventCallBackFunc = callBack;
    
    self.layer = [CAMetalLayer layer];
    assert(self.layer);

    return self;
}

- (void)sizeHasChange
{
    self.metalLayer.drawableSize = [self convertRectToBacking:self.frame].size;
}

- (CAMetalLayer *)metalLayer
{
    return (CAMetalLayer*)self.layer;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

// Events

- (void)keyDown:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);

    KeyDownEvent keyDownEvent(*m_gfxWindow, static_cast<unsigned char>(event.keyCode), event.ARepeat);
    m_eventCallBackFunc(keyDownEvent);
}

- (void)keyUp:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);
    
    KeyUpEvent keyUpEvent(*m_gfxWindow, static_cast<unsigned char>(event.keyCode));
    m_eventCallBackFunc(keyUpEvent);
}

- (void)mouseDown:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);
    
    NSPoint mouseLocation = event.window.mouseLocationOutsideOfEventStream;
    MouseDownEvent mouseDownEvent(*m_gfxWindow, 1, static_cast<int>(mouseLocation.x), static_cast<int>(mouseLocation.y));
    m_eventCallBackFunc(mouseDownEvent);
}

- (void)mouseUp:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);

    NSPoint mouseLocation = event.window.mouseLocationOutsideOfEventStream;
    MouseUpEvent mouseUpEvent(*m_gfxWindow, 1, static_cast<int>(mouseLocation.x), static_cast<int>(mouseLocation.y));
    m_eventCallBackFunc(mouseUpEvent);
}

- (void)rightMouseDown:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);

    NSPoint mouseLocation = event.window.mouseLocationOutsideOfEventStream;
    MouseDownEvent mouseDownEvent(*m_gfxWindow, 2, static_cast<int>(mouseLocation.x), static_cast<int>(mouseLocation.y));
    m_eventCallBackFunc(mouseDownEvent);
}

- (void)rightMouseUp:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);
    
    NSPoint mouseLocation = event.window.mouseLocationOutsideOfEventStream;
    MouseUpEvent mouseUpEvent(*m_gfxWindow, 2, static_cast<int>(mouseLocation.x),  static_cast<int>(mouseLocation.y));
    m_eventCallBackFunc(mouseUpEvent);
}

- (void)mouseMoved:(NSEvent *)event
{
    assert(m_eventCallBackFunc == true);

    NSPoint mouseLocation = event.window.mouseLocationOutsideOfEventStream;
    MouseMoveEvent mouseMoveEvent(*m_gfxWindow, static_cast<int>(mouseLocation.x), static_cast<int>(mouseLocation.y));
    m_eventCallBackFunc(mouseMoveEvent);
}


@end