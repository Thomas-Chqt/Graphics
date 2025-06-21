/*
 * ---------------------------------------------------
 * VulkanRenderPass.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/05 06:43:00
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/RenderPass.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanEnums.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <optional>
    #include <vector>
    #include <cstdint>
    #include <array>
    namespace ext = std;
#endif

namespace gfx
{

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& device, const RenderPass::Descriptor& desc)
    : m_device(&device), m_colorAttachmentsDesc(desc.colorAttachments), m_depthAttachmentDesc(desc.depthAttachment)
{
    ext::vector<vk::AttachmentDescription> attachments;

    ext::vector<vk::AttachmentReference> colorAttachmentRefs;
    for (uint32_t i = 0; auto& attachmentDescriptor : desc.colorAttachments)
    {
        auto initialLayout = vk::ImageLayout::eUndefined;
        if (attachmentDescriptor.loadAction == LoadAction::load)
            initialLayout = vk::ImageLayout::eColorAttachmentOptimal;

        attachments.push_back(vk::AttachmentDescription{
            .format = toVkFormat(attachmentDescriptor.format),
            .samples = vk::SampleCountFlagBits::e1,
            .loadOp = toVkAttachmentLoadOp(attachmentDescriptor.loadAction),
            .storeOp = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = initialLayout,
            .finalLayout = vk::ImageLayout::eGeneral});

        colorAttachmentRefs.push_back(vk::AttachmentReference{
            .attachment = i,
            .layout = vk::ImageLayout::eColorAttachmentOptimal});
    }

    ext::optional<vk::AttachmentReference> depthAttachmentRef;
    if (desc.depthAttachment.has_value())
    {
        auto& attachmentDescriptor = desc.depthAttachment.value();

        auto initialLayout = vk::ImageLayout::eUndefined;
        if (attachmentDescriptor.loadAction == LoadAction::load)
            initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        attachments.push_back(vk::AttachmentDescription{
            .format = toVkFormat(attachmentDescriptor.format),
            .samples = vk::SampleCountFlagBits::e1,
            .loadOp = toVkAttachmentLoadOp(attachmentDescriptor.loadAction),
            .storeOp = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = initialLayout,
            .finalLayout = vk::ImageLayout::eGeneral});

        depthAttachmentRef = vk::AttachmentReference{
            .attachment = (uint32_t)colorAttachmentRefs.size(),
            .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal};
    }

    ext::array<vk::SubpassDescription, 1> subpasses;
    {
        vk::SubpassDescription subpass = {
            .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        };
        subpass.setColorAttachments(colorAttachmentRefs);
        if (depthAttachmentRef.has_value())
            subpass.setPDepthStencilAttachment(&depthAttachmentRef.value());
        subpasses[0] = subpass;
    }

    vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::AccessFlags srcAccessMask = {};
    if (ext::ranges::any_of(desc.colorAttachments, [](const auto& a){return a.loadAction == LoadAction::load;}))
        srcAccessMask |= vk::AccessFlagBits::eColorAttachmentWrite;
    vk::AccessFlags dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;


    if (desc.depthAttachment.has_value()) {
        srcStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dstStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;

        if (desc.depthAttachment->loadAction == LoadAction::load)
            srcAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        dstAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    }

    ext::array<vk::SubpassDependency, 1> dependencies = {
        vk::SubpassDependency{
            .srcSubpass = vk::SubpassExternal,
            .dstSubpass = 0,
            .srcStageMask = srcStageMask,
            .dstStageMask = dstStageMask,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
        }};

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachments(attachments);
    renderPassInfo.setSubpasses(subpasses);
    renderPassInfo.setDependencies(dependencies);

    m_vkRenderPass = device.vkDevice().createRenderPass(renderPassInfo);
}

VulkanRenderPass::~VulkanRenderPass()
{
    m_device->vkDevice().destroyRenderPass(m_vkRenderPass);
}

}
