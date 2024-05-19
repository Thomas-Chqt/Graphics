/*
 * ---------------------------------------------------
 * WindowDelegate.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/26 13:58:58
 * ---------------------------------------------------
 */

#ifndef WINDOWDELEGATE_H
# define WINDOWDELEGATE_H

#import <AppKit/AppKit.h>

#include "UtilsCPP/Func.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"

@interface WindowDelegate : NSObject<NSWindowDelegate>

- (instancetype)initWithGFXWindow:(gfx::Window&)win andCallBack:(const utils::Func<void(gfx::Event&)>&)callBack;

@end

#endif // WINDOWDELEGATE_H