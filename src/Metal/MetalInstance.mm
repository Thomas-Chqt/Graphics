/*
 * ---------------------------------------------------
 * MetalInstance.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:18:35
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/Instance.hpp"

#include "Metal/MetalInstance.hpp"
#include "Metal/MetalDevice.hpp"

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
namespace ext = utl;
#else
    #include <memory>
namespace ext = std;
#endif

namespace gfx
{

MetalInstance::MetalInstance(const Instance::Descriptor& desc)
{
}

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc)
{
    return ext::make_unique<MetalDevice>(desc);
}

} // namespace gfx
