#include "gfx_tracy_private.hpp"

#include "Graphics/Device.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <cassert>
#include <cstdint>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#if defined(TRACY_ENABLE)
#include <tracy/TracyVulkan.hpp>
#endif

namespace gfx::tracy
{

namespace
{

struct VulkanTracyGfxData
{
#if defined(TRACY_ENABLE)
    ::tracy::VkCtx* context = nullptr;
    std::mutex sourceLocationsMtx;
    std::deque<::tracy::SourceLocationData> sourceLocations;
    std::mutex activeZonesMtx;
    std::map<CommandBuffer*, std::vector<std::unique_ptr<::tracy::VkCtxScope>>> activeZones;
#endif
};

VulkanTracyGfxData& vulkanData(TracyGfxCtx& context)
{
    assert(context.backend == Backend::vulkan);
    assert(context.backendData);
    return *static_cast<VulkanTracyGfxData*>(context.backendData);
}

const VulkanDevice& vulkanDevice(const Device& device)
{
    auto* vulkanDevice = dynamic_cast<const VulkanDevice*>(&device);
    assert(vulkanDevice);
    return *vulkanDevice;
}

VulkanCommandBuffer& vulkanCommandBuffer(CommandBuffer& commandBuffer)
{
    auto* vulkanCommandBuffer = dynamic_cast<VulkanCommandBuffer*>(&commandBuffer);
    assert(vulkanCommandBuffer);
    return *vulkanCommandBuffer;
}

#if defined(TRACY_ENABLE)
const ::tracy::SourceLocationData* storeSourceLocation(VulkanTracyGfxData& data, TracyGfxSourceLocation sourceLocation)
{
    std::scoped_lock lock(data.sourceLocationsMtx);
    return &data.sourceLocations.emplace_back(::tracy::SourceLocationData{
        sourceLocation.name,
        sourceLocation.function,
        sourceLocation.file,
        sourceLocation.line,
        sourceLocation.color});
}
#endif

void destroyVulkanTracyGfxContext(TracyGfxCtx* context)
{
    assert(context);
    auto& data = vulkanData(*context);
#if defined(TRACY_ENABLE)
    ::tracy::DestroyVkContext(data.context);
#endif
    delete &data;
    delete context;
}

void collectVulkanTracyGfxContext(TracyGfxCtx* context)
{
    assert(context);
#if defined(TRACY_ENABLE)
    auto& data = vulkanData(*context);
    data.context->Collect(VK_NULL_HANDLE);
#else
    (void)context;
#endif
}

void beginVulkanTracyGfxZone(TracyGfxCtx& context, CommandBuffer& commandBuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
#if defined(TRACY_ENABLE)
    auto& data = vulkanData(context);
    auto& vkCommandBuffer = vulkanCommandBuffer(commandBuffer);
    auto sourceLocationData = storeSourceLocation(data, sourceLocation);

    auto scope = std::make_unique<::tracy::VkCtxScope>(data.context, sourceLocationData, vkCommandBuffer.vkCommandBuffer(), active);
    std::scoped_lock lock(data.activeZonesMtx);
    data.activeZones[&commandBuffer].push_back(std::move(scope));
#else
    (void)context;
    (void)commandBuffer;
    (void)sourceLocation;
    (void)active;
#endif
}

void endVulkanTracyGfxZone(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
#if defined(TRACY_ENABLE)
    auto& data = vulkanData(context);
    std::scoped_lock lock(data.activeZonesMtx);
    auto zonesIt = data.activeZones.find(&commandBuffer);
    assert(zonesIt != data.activeZones.end());
    assert(zonesIt->second.empty() == false);
    zonesIt->second.pop_back();
    if (zonesIt->second.empty())
        data.activeZones.erase(zonesIt);
#else
    (void)context;
    (void)commandBuffer;
#endif
}

void beginVulkanTracyGfxRenderPass(TracyGfxCtx& context, CommandBuffer& commandBuffer, const Framebuffer& framebuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    beginVulkanTracyGfxZone(context, commandBuffer, sourceLocation, active);
    vulkanCommandBuffer(commandBuffer).beginRenderPass(framebuffer);
}

void endVulkanTracyGfxRenderPass(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    vulkanCommandBuffer(commandBuffer).endRenderPass();
    endVulkanTracyGfxZone(context, commandBuffer);
}

void beginVulkanTracyGfxBlitPass(TracyGfxCtx& context, CommandBuffer& commandBuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    beginVulkanTracyGfxZone(context, commandBuffer, sourceLocation, active);
    vulkanCommandBuffer(commandBuffer).beginBlitPass();
}

void endVulkanTracyGfxBlitPass(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    vulkanCommandBuffer(commandBuffer).endBlitPass();
    endVulkanTracyGfxZone(context, commandBuffer);
}

} // namespace

TracyGfxCtx* createVulkanTracyGfxContext(const Device& device, std::string_view name)
{
    const auto& vkDevice = vulkanDevice(device);
    auto* data = new VulkanTracyGfxData();

#if defined(TRACY_ENABLE)
    data->context = ::tracy::CreateVkContext(
        vkDevice.instance().vkInstance(),
        static_cast<VkPhysicalDevice>(vkDevice.physicalDevice()),
        vkDevice.vkDevice(),
        reinterpret_cast<PFN_vkGetInstanceProcAddr>(VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr),
        reinterpret_cast<PFN_vkGetDeviceProcAddr>(VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr));

    if (name.empty() == false)
    {
        assert(name.size() <= std::numeric_limits<std::uint16_t>::max());
        data->context->Name(name.data(), static_cast<std::uint16_t>(name.size()));
    }
#else
    (void)vkDevice;
    (void)name;
#endif

    return new TracyGfxCtx{
        .backend = Backend::vulkan,
        .backendData = data,
        .destroy = destroyVulkanTracyGfxContext,
        .collect = collectVulkanTracyGfxContext,
        .beginZone = beginVulkanTracyGfxZone,
        .endZone = endVulkanTracyGfxZone,
        .beginRenderPass = beginVulkanTracyGfxRenderPass,
        .endRenderPass = endVulkanTracyGfxRenderPass,
        .beginBlitPass = beginVulkanTracyGfxBlitPass,
        .endBlitPass = endVulkanTracyGfxBlitPass};
}

} // namespace gfx::tracy
