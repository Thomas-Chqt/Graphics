#pragma once

#include "Vulkan/VulkanPassDescriptor.hpp"

#include <tracy/TracyVulkan.hpp>

#include <cassert>
#include <optional>
#include <utility>

namespace gfx::tracy::detail
{

class VulkanZoneImpl
{
public:
    VulkanZoneImpl(TracyGfxCtx* context, PassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation)
        : m_context(reinterpret_cast<::tracy::VkCtx*>(context)), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
          m_descriptor(descriptor),
          m_sourceLocation(sourceLocation)
    {
        assert(m_context);
        assert(m_sourceLocation);

        if (auto* renderDescriptor = dynamic_cast<VulkanRenderPassDescriptor*>(&m_descriptor))
            renderDescriptor->setBeginCallback(passBegin, this);
        else if (auto* blitDescriptor = dynamic_cast<VulkanBlitPassDescriptor*>(&m_descriptor))
            blitDescriptor->setBeginCallback(passBegin, this);
        else
            std::unreachable();
    }

    VulkanZoneImpl(const VulkanZoneImpl&) = delete;
    VulkanZoneImpl(VulkanZoneImpl&&) = delete;

    ~VulkanZoneImpl()
    {
        if (auto* renderDescriptor = dynamic_cast<VulkanRenderPassDescriptor*>(&m_descriptor))
            renderDescriptor->clearBeginCallback();
        else if (auto* blitDescriptor = dynamic_cast<VulkanBlitPassDescriptor*>(&m_descriptor))
            blitDescriptor->clearBeginCallback();
        else
            std::unreachable();

        m_scope.reset();
    }

    VulkanZoneImpl& operator=(const VulkanZoneImpl&) = delete;
    VulkanZoneImpl& operator=(VulkanZoneImpl&&) = delete;

private:
    static void passBegin(void* userData, const vk::CommandBuffer& commandBuffer)
    {
        auto& self = *static_cast<VulkanZoneImpl*>(userData);
        assert(self.m_scope.has_value() == false);
        self.m_scope.emplace(self.m_context, self.m_sourceLocation, static_cast<VkCommandBuffer>(commandBuffer), true);
    }

private:
    ::tracy::VkCtx* m_context;
    PassDescriptor& m_descriptor;
    const ::tracy::SourceLocationData* m_sourceLocation;
    std::optional<::tracy::VkCtxScope> m_scope;
};

static_assert(sizeof(VulkanZoneImpl) <= TracyGFXZoneState::storageSize);
static_assert(alignof(VulkanZoneImpl) <= TracyGFXZoneState::storageAlignment);

}
