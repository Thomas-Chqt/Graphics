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
#ifdef USING_OPENGL
    #include "Window/MacOS/Views/OpenGLView.h"
#endif
#ifdef USING_METAL
    #include "Window/MacOS/Views/MetalView.h"
#endif

using utils::Func;

namespace gfx
{

#ifdef USING_OPENGL
void MacOSWindow::useOpenGL() { @autoreleasepool
{
    m_contentView = [[[OpenGLView alloc] initWithGFXWindow:*this
                                               andCallBack:Func<void(Event&)>(*this, &MacOSWindow::eventCallBack)] autorelease];

    ((OpenGLView*)m_contentView).wantsBestResolutionOpenGLSurface = YES;
    
    m_nsWindow.contentView = (OpenGLView*)m_contentView;
    [m_nsWindow makeFirstResponder:(OpenGLView*)m_contentView];

    makeOpenGlContextCurrent();

    [m_contentView sizeHasChange];

    logDebug << "Content view set to OpenGL view" << std::endl;
}}

void MacOSWindow::makeOpenGlContextCurrent()
{
    [(OpenGLView*)m_contentView makeOpenGLContextCurrent];
}

void MacOSWindow::openGLSwapBuffer() { @autoreleasepool
{
    [((OpenGLView*)m_contentView) swapOpenGLBuffers];
}}
#endif

#ifdef USING_METAL
void MacOSWindow::useMetal() { @autoreleasepool
{
    m_contentView = [[[MetalView alloc] initWithGFXWindow:*this
                                              andCallBack:Func<void(Event&)>(*this, &MacOSWindow::eventCallBack)] autorelease];

    ((MetalView*)m_contentView).wantsLayer = YES;
    
    m_nsWindow.contentView = (OpenGLView*)m_contentView;
    [m_nsWindow makeFirstResponder:(OpenGLView*)m_contentView];

    [m_contentView sizeHasChange];

    logDebug << "Content view set to Metal view" << std::endl;
}}

CAMetalLayer* MacOSWindow::getMetalLayer() { @autoreleasepool
{
    return [((MetalView*)m_contentView) metalLayer];
}}
#endif

MacOSWindow::~MacOSWindow() { @autoreleasepool
{
    [m_nsWindow close];
    [m_windowDelegate release];
    logDebug << "MacOSWindow deleted" << std::endl;
}}

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

}