/*
 * ---------------------------------------------------
 * OpenGLView.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/30 23:49:36
 * ---------------------------------------------------
 */

#ifndef OPENGLVIEW_H
# define OPENGLVIEW_H

#include "Window/MacOS/Views/MacOSWindowView.h"

#import <AppKit/AppKit.h>

@interface OpenGLView : NSOpenGLView<MacOSWindowView>

- (void)makeOpenGLContextCurrent;
- (void)swapOpenGLBuffers;

@end

#endif // OPENGLVIEW_H