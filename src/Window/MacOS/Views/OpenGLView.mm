/*
 * ---------------------------------------------------
 * OpenGLView.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/27 10:48:24
 * ---------------------------------------------------
 */

#include "Window/MacOS/Views/OpenGLView.h"
#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"

using gfx::KeyDownEvent;
using gfx::KeyUpEvent;
using gfx::MouseDownEvent;
using gfx::MouseUpEvent;
using gfx::MouseMoveEvent;

@implementation OpenGLView
{
    gfx::Window* m_gfxWindow;
    utils::Func<void(gfx::Event&)> m_eventCallBackFunc;
}

- (instancetype)initWithGFXWindow:(gfx::Window&)win andCallBack:(const utils::Func<void(gfx::Event&)>&)callBack
{
    [super init];

    m_gfxWindow = &win;
    m_eventCallBackFunc = callBack;
    
    NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 32, 
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        0
	};
	
	NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
    self.pixelFormat = pixelFormat;

    self.openGLContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil] autorelease];

    return self;
}

- (void)sizeHasChange
{
    [self.openGLContext update];
}

- (void)makeOpenGLContextCurrent
{
    [self.openGLContext makeCurrentContext];
}

- (void)swapOpenGLBuffers
{
    [self.openGLContext flushBuffer];
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