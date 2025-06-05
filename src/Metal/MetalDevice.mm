/*
 * ---------------------------------------------------
 * MetalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 07:35:19
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalRenderPass.hpp"
#include "Metal/MetalSwapchain.hpp"

#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <cassert>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(const MetalPhysicalDevice& phyDevice, const Device::Descriptor& desc)
    : m_physicalDevice(&phyDevice)
{
    @autoreleasepool
    {
        m_mtlDevice = [phyDevice.mtlDevice() retain];
        assert(phyDevice.isSuitable(desc));
        for (auto& [_, count] : desc.queues)
            m_queues.push_back([phyDevice.mtlDevice() newCommandQueue]);
    }
}

ext::unique_ptr<RenderPass> MetalDevice::newRenderPass(const RenderPass::Descriptor& desc) const
{
    return ext::make_unique<MetalRenderPass>(desc);
}

ext::unique_ptr<Swapchain> MetalDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<MetalSwapchain>(*this, desc);
}

MetalDevice::~MetalDevice()
{
    @autoreleasepool
    {
        for (auto& queue : m_queues)
            [queue release];
        [m_mtlDevice release];
    }
}
}
