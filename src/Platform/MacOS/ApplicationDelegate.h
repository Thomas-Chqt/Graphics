/*
 * ---------------------------------------------------
 * ApplicationDelegate.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/15 15:30:13
 * ---------------------------------------------------
 */

#ifndef APPLICATIONDELEGATE_H
# define APPLICATIONDELEGATE_H

#import <AppKit/AppKit.h>

#include "UtilsCPP/Func.hpp"
#include "Graphics/Event.hpp"

@interface ApplicationDelegate : NSObject<NSApplicationDelegate>

- (instancetype)initWithCallBack:(const utils::Func<void(gfx::Event&)>&)callBack;

@end

#endif // APPLICATIONDELEGATE_H