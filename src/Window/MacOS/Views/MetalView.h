/*
 * ---------------------------------------------------
 * MetalView.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/26 17:35:39
 * ---------------------------------------------------
 */

#ifndef METALVIEW_H
# define METALVIEW_H

#include "Window/MacOS/Views/MacOSWindowView.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

@interface MetalView : NSView<MacOSWindowView>

@property(readonly) CAMetalLayer* metalLayer;

@end

#endif // METALVIEW_H