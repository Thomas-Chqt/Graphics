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
    namespace ext = utl;
#else
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(const Device::Descriptor&) { @autoreleasepool
{
    m_mtlDevice = MTLCreateSystemDefaultDevice();
    if (m_mtlDevice == nil)
        throw ext::runtime_error("MTLCreateSystemDefaultDevice failed");

    m_commandQueue = [m_mtlDevice newCommandQueue];
    if (m_commandQueue == nil)
        throw ext::runtime_error("newCommandQueue failed");
}}

}
