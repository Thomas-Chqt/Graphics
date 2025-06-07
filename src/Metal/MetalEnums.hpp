/*
 * ---------------------------------------------------
 * MetalEnums.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:17:50
 * ---------------------------------------------------
 */

#ifndef METALENUMS_HPP
#define METALENUMS_HPP

#include "Graphics/Enums.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    using MTLPixelFormat = unsigned long;
    using MTLLoadAction = unsigned long;
#endif // __OBJC__

namespace gfx
{

MTLPixelFormat toMTLPixelFormat(PixelFormat);
PixelFormat toPixelFormat(MTLPixelFormat);

MTLLoadAction toMTLLoadAction(LoadAction);

}

#endif // METALENUMS_HPP
