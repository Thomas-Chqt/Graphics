#ifndef VULKANPASSDESCRIPTOR_HPP
#define VULKANPASSDESCRIPTOR_HPP

#include "Graphics/PassDescriptor.hpp"

#include <cassert>
#include <utility>

namespace gfx
{

class VulkanRenderPassDescriptor final : public RenderPassDescriptor
{
public:
    using BeginCallback = void (*)(void*, const vk::CommandBuffer&);

    const std::vector<Attachment>& colorAttachments() const override { return m_colorAttachments; }
    void setColorAttachments(std::vector<Attachment> attachments) override { m_colorAttachments = std::move(attachments); }

    const std::optional<Attachment>& depthAttachment() const override { return m_depthAttachment; }
    void setDepthAttachment(std::optional<Attachment> attachment) override { m_depthAttachment = std::move(attachment); }

    void setBeginCallback(BeginCallback callback, void* userData)
    {
        assert(m_beginCallback == nullptr);
        assert(callback != nullptr);
        assert(userData != nullptr);
        m_beginCallback = callback;
        m_beginCallbackUserData = userData;
    }
    void clearBeginCallback()
    {
        assert(m_beginCallback != nullptr);
        m_beginCallback = nullptr;
        m_beginCallbackUserData = nullptr;
    }
    void invokeBeginCallback(const vk::CommandBuffer& commandBuffer)
    {
        if (m_beginCallback)
            m_beginCallback(m_beginCallbackUserData, commandBuffer);
    }

private:
    std::vector<Attachment> m_colorAttachments;
    std::optional<Attachment> m_depthAttachment;
    BeginCallback m_beginCallback = nullptr;
    void* m_beginCallbackUserData = nullptr;
};

class VulkanBlitPassDescriptor final : public BlitPassDescriptor
{
public:
    using BeginCallback = void (*)(void*, const vk::CommandBuffer&);

    void setBeginCallback(BeginCallback callback, void* userData)
    {
        assert(m_beginCallback == nullptr);
        assert(callback != nullptr);
        assert(userData != nullptr);
        m_beginCallback = callback;
        m_beginCallbackUserData = userData;
    }
    void clearBeginCallback()
    {
        assert(m_beginCallback != nullptr);
        m_beginCallback = nullptr;
        m_beginCallbackUserData = nullptr;
    }
    void invokeBeginCallback(const vk::CommandBuffer& commandBuffer)
    {
        if (m_beginCallback)
            m_beginCallback(m_beginCallbackUserData, commandBuffer);
    }

private:
    BeginCallback m_beginCallback = nullptr;
    void* m_beginCallbackUserData = nullptr;
};

} // namespace gfx

#endif // VULKANPASSDESCRIPTOR_HPP
