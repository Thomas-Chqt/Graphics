#include "gfx_tracy_private.hpp"

#if defined(TRACY_ENABLE)

#include "Graphics/BlitPassDescriptor.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/RenderPassDescriptor.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanPassDescriptor.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <cassert>
#include <memory>
#include <variant>

#include <tracy/TracyVulkan.hpp>

namespace gfx::tracy
{

namespace
{

struct VulkanGraphicsData
{
    ::tracy::VkCtx* context = nullptr;
};

struct VulkanZoneData
{
    using Descriptor = std::variant<VulkanRenderPassDescriptor*, VulkanBlitPassDescriptor*>;

    VulkanZoneData(::tracy::VkCtx* context, VulkanRenderPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
        : m_context(context), m_descriptor(&descriptor), m_sourceLocation(sourceLocation), m_active(active)
    {
        descriptor.setBeginCallback([this](const vk::CommandBuffer& commandBuffer) { begin(commandBuffer); });
    }

    VulkanZoneData(::tracy::VkCtx* context, VulkanBlitPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
        : m_context(context), m_descriptor(&descriptor), m_sourceLocation(sourceLocation), m_active(active)
    {
        descriptor.setBeginCallback([this](const vk::CommandBuffer& commandBuffer) { begin(commandBuffer); });
    }

    ~VulkanZoneData()
    {
        m_scope.reset();
        std::visit([](auto* descriptor) { descriptor->clearBeginCallback(); }, m_descriptor);
    }

    void begin(const vk::CommandBuffer& commandBuffer)
    {
        assert(m_scope == nullptr);
        m_scope = std::make_unique<::tracy::VkCtxScope>(m_context, m_sourceLocation, commandBuffer, m_active);
    }

private:
    ::tracy::VkCtx* m_context;
    Descriptor m_descriptor;
    const ::tracy::SourceLocationData* m_sourceLocation;
    bool m_active;
    std::unique_ptr<::tracy::VkCtxScope> m_scope;
};

VulkanGraphicsData& vulkanData(GraphicsContext& context)
{
    assert(context.backend == Backend::vulkan);
    assert(context.backendData);
    return *static_cast<VulkanGraphicsData*>(context.backendData);
}

const VulkanDevice& vulkanDevice(const Device& device)
{
    auto* result = dynamic_cast<const VulkanDevice*>(&device);
    assert(result);
    return *result;
}

void destroyVulkanGraphicsContext(GraphicsContext* context)
{
    assert(context);
    auto& data = vulkanData(*context);
    ::tracy::DestroyVkContext(data.context);
    delete &data;
    delete context;
}

void collectVulkanGraphicsContext(GraphicsContext* context)
{
    assert(context);
    vulkanData(*context).context->Collect(VK_NULL_HANDLE);
}

void* createVulkanRenderZone(GraphicsContext& context, RenderPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
{
    auto* vulkanDescriptor = dynamic_cast<VulkanRenderPassDescriptor*>(&descriptor);
    assert(vulkanDescriptor);
    return new VulkanZoneData(vulkanData(context).context, *vulkanDescriptor, sourceLocation, active);
}

void* createVulkanBlitZone(GraphicsContext& context, BlitPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
{
    auto* vulkanDescriptor = dynamic_cast<VulkanBlitPassDescriptor*>(&descriptor);
    assert(vulkanDescriptor);
    return new VulkanZoneData(vulkanData(context).context, *vulkanDescriptor, sourceLocation, active);
}

void destroyVulkanZone(void* zone)
{
    assert(zone);
    delete static_cast<VulkanZoneData*>(zone);
}

} // namespace

GraphicsContext* createVulkanGraphicsContext(const Device& device)
{
    const auto& vkDevice = vulkanDevice(device);
    auto* data = new VulkanGraphicsData{
        .context = ::tracy::CreateVkContext(
            vkDevice.instance().vkInstance(),
            static_cast<VkPhysicalDevice>(vkDevice.physicalDevice()),
            vkDevice.vkDevice(),
            reinterpret_cast<PFN_vkGetInstanceProcAddr>(VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr),
            reinterpret_cast<PFN_vkGetDeviceProcAddr>(VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr))
    };

    return new GraphicsContext{
        .backend = Backend::vulkan,
        .backendData = data,
        .destroy = destroyVulkanGraphicsContext,
        .collect = collectVulkanGraphicsContext,
        .createRenderZone = createVulkanRenderZone,
        .createBlitZone = createVulkanBlitZone,
        .destroyZone = destroyVulkanZone};
}

} // namespace gfx::tracy

#endif
