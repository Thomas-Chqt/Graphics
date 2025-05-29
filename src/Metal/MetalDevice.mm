/*
 * ---------------------------------------------------
 * MetalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 07:35:19
 * ---------------------------------------------------
 */

#include "Metal/MetalDevice.hpp"
#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
#else
#endif

namespace gfx
{

MetalDevice::MetalDevice(const Device::Descriptor&) { @autoreleasepool
{
}}

}
