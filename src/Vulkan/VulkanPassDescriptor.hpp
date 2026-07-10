#ifndef VULKANPASSDESCRIPTOR_HPP
#define VULKANPASSDESCRIPTOR_HPP

#include "Graphics/BlitPassDescriptor.hpp"
#include "Graphics/RenderPassDescriptor.hpp"

#include <cassert>
#include <functional>
#include <utility>

namespace gfx
{

class VulkanRenderPassDescriptor final : public RenderPassDescriptor
{
public:
    using BeginCallback = std::function<void(const vk::CommandBuffer&)>;

    const std::vector<Attachment>& colorAttachments() const override { return m_colorAttachments; }
    void setColorAttachments(std::vector<Attachment> attachments) override { m_colorAttachments = std::move(attachments); }

    const std::optional<Attachment>& depthAttachment() const override { return m_depthAttachment; }
    void setDepthAttachment(std::optional<Attachment> attachment) override { m_depthAttachment = std::move(attachment); }

    void setBeginCallback(BeginCallback callback)
    {
        assert(m_beginCallback == nullptr);
        m_beginCallback = std::move(callback);
    }
    void clearBeginCallback()
    {
        assert(m_beginCallback != nullptr);
        m_beginCallback = nullptr;
    }
    void invokeBeginCallback(const vk::CommandBuffer& commandBuffer)
    {
        if (m_beginCallback)
            m_beginCallback(commandBuffer);
    }

private:
    std::vector<Attachment> m_colorAttachments;
    std::optional<Attachment> m_depthAttachment;
    BeginCallback m_beginCallback;
};

class VulkanBlitPassDescriptor final : public BlitPassDescriptor
{
public:
    using BeginCallback = std::function<void(const vk::CommandBuffer&)>;

    void setBeginCallback(BeginCallback callback)
    {
        assert(m_beginCallback == nullptr);
        m_beginCallback = std::move(callback);
    }
    void clearBeginCallback()
    {
        assert(m_beginCallback != nullptr);
        m_beginCallback = nullptr;
    }
    void invokeBeginCallback(const vk::CommandBuffer& commandBuffer)
    {
        if (m_beginCallback)
            m_beginCallback(commandBuffer);
    }

private:
    BeginCallback m_beginCallback;
};

} // namespace gfx

#endif // VULKANPASSDESCRIPTOR_HPP
