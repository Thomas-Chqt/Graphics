/*
 * ---------------------------------------------------
 * NSWindow.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 16:51:40
 * ---------------------------------------------------
 */

#ifndef NSWINDOW_H
# define NSWINDOW_H

#include <objc/NSObject.h>

#include "IMockNSWindow.hpp"

using gfx_tests::IMockNSView;
using gfx_tests::IMockNSWindow;

@interface NSView : NSObject

- (instancetype) initWithMockNSView:(IMockNSView*)view;

@property(nonatomic, assign) BOOL wantsLayer;
@property(nonatomic, assign) void* layer;

@end


@interface NSWindow : NSObject

- (instancetype) initWithMockNSWindow:(IMockNSWindow*)win;

@property(nonatomic, assign) NSView* contentView;

@end

#endif // NSWINDOW_H