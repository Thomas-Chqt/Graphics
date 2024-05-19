/*
 * ---------------------------------------------------
 * MacOSWindowView.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/26 13:58:58
 * ---------------------------------------------------
 */

#ifndef MACOSWINDOWVIEW_HPP
# define MACOSWINDOWVIEW_HPP

#include "Graphics/Event.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"

@protocol MacOSWindowView
@required

- (instancetype)initWithGFXWindow:(gfx::Window&)win andCallBack:(const utils::Func<void(gfx::Event&)>&)callBack;
- (void)sizeHasChange;

@end

#endif // MACOSWINDOWVIEW_HPP