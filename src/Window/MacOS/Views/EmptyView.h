/*
 * ---------------------------------------------------
 * EmptyView.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/17 13:40:36
 * ---------------------------------------------------
 */

#ifndef EMPTYVIEW_H
# define EMPTYVIEW_H

#import <AppKit/AppKit.h>

#include "Window/MacOS/Views/MacOSWindowView.h"

@interface EmptyView : NSView<MacOSWindowView>
@end

#endif // EMPTYVIEW_H