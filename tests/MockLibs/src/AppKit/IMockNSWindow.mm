/*
 * ---------------------------------------------------
 * IMockNSWindow.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 17:08:51
 * ---------------------------------------------------
 */

#include "AppKit/NSWindow.h"
#include "QuartzCore/CAMetalLayer.h"

@implementation NSView
{
    gfx_tests::IMockNSView* m_mockNSView;
}

- (instancetype) initWithMockNSView:(IMockNSView*)view
{
    [super init];
    m_mockNSView = (gfx_tests::IMockNSView*)view;
    return self;
}

- (void)setWantsLayer:(BOOL)wantsLayer
{
    m_mockNSView->setWantsLayer((int)wantsLayer);
}

- (void*) layer
{
    return (IMockCAMetalLayer*)m_mockNSView->getLayer()->caMetalLayer;
}

- (void) setLayer:(void *)layer
{
    m_mockNSView->setLayer(layer);
}

@end


@implementation NSWindow
{
    gfx_tests::IMockNSWindow* m_mockNSWindow;
}

- (instancetype) initWithMockNSWindow:(IMockNSWindow*)win
{
    [super init];
    m_mockNSWindow = (gfx_tests::IMockNSWindow*)win;
    return self;
}

- (NSView*) contentView
{
    return (NSView*)m_mockNSWindow->getContentView()->nsView;
}

@end


namespace gfx_tests
{

IMockNSView::IMockNSView()
{
    nsView = [[NSView alloc] initWithMockNSView: this];
}

IMockNSView::~IMockNSView()
{
    [(NSView*)nsView release];
}


IMockNSWindow::IMockNSWindow()
{
    nsWindow = [[NSWindow alloc] initWithMockNSWindow: this];
}

IMockNSWindow::~IMockNSWindow()
{
    [(NSWindow*)nsWindow release];
}

}