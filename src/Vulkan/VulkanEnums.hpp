/*
 * ---------------------------------------------------
 * VulkanEnums.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:26:22
 * ---------------------------------------------------
 */

#ifndef VULKANENUMS_HPP
#define VULKANENUMS_HPP

#include "Graphics/Enums.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace gfx
{

constexpr vk::Format toVkFormat(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::BGRA8Unorm:
        return vk::Format::eB8G8R8A8Unorm;
    case PixelFormat::BGRA8Unorm_sRGB:
        return vk::Format::eB8G8R8A8Srgb;
    case PixelFormat::Depth32Float:
        return vk::Format::eD32Sfloat;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr PixelFormat toPixelFormat(vk::Format fmt)
{
    switch (fmt)
    {
    case vk::Format::eB8G8R8A8Unorm:
        return PixelFormat::BGRA8Unorm;
    case vk::Format::eB8G8R8A8Srgb:
        return PixelFormat::BGRA8Unorm_sRGB;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr vk::ColorSpaceKHR toVkColorSpaceKHR(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::BGRA8Unorm:
    case PixelFormat::BGRA8Unorm_sRGB:
        return vk::ColorSpaceKHR::eSrgbNonlinear;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr vk::PresentModeKHR toVkPresentModeKHR(PresentMode pmd)
{
    switch (pmd)
    {
    case PresentMode::fifo:
        return vk::PresentModeKHR::eFifo;
    case PresentMode::mailbox:
        return vk::PresentModeKHR::eMailbox;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr PresentMode toPresentMode(vk::PresentModeKHR pmd)
{
    switch (pmd)
    {
    case vk::PresentModeKHR::eFifo:
        return PresentMode::fifo;
    case vk::PresentModeKHR::eMailbox:
        return PresentMode::mailbox;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr vk::AttachmentLoadOp toVkAttachmentLoadOp(LoadAction loa)
{
    switch (loa)
    {
    case LoadAction::load:
        return vk::AttachmentLoadOp::eLoad;
    case LoadAction::clear:
        return vk::AttachmentLoadOp::eClear;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr vk::Format toVkFormat(VertexAttributeFormat fmt)
{
    switch (fmt)
    {
    case VertexAttributeFormat::float2:
        return vk::Format::eR32G32Sfloat;
    case VertexAttributeFormat::float3:
        return vk::Format::eR32G32B32Sfloat;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr vk::BufferUsageFlags toVkBufferUsageFlags(BufferUsages use)
{
    vk::BufferUsageFlags vkUsages;
    
    if (use & BufferUsage::vertexBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eVertexBuffer;
    if (use & BufferUsage::indexBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eIndexBuffer;
    if (use & BufferUsage::uniformBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eUniformBuffer;

    return vkUsages;
}

constexpr vk::DescriptorType toVkDescriptorType(BindingType tpe)
{
    switch (tpe)
    {
    case BindingType::uniformBuffer:
        return vk::DescriptorType::eUniformBuffer;
    default:
        throw ext::runtime_error("not implemented");
    }

}

constexpr vk::ShaderStageFlags toVkShaderStageFlags(BindingUsages use)
{
    vk::ShaderStageFlags vkShaderStageFlags;

    if (use & BindingUsage::vertexRead)
        vkShaderStageFlags |= vk::ShaderStageFlagBits::eVertex;
    if (use & BindingUsage::vertexWrite)
        vkShaderStageFlags |= vk::ShaderStageFlagBits::eVertex;
    if (use & BindingUsage::fragmentRead)
        vkShaderStageFlags |= vk::ShaderStageFlagBits::eFragment;
    if (use & BindingUsage::fragmentWrite)
        vkShaderStageFlags |= vk::ShaderStageFlagBits::eFragment;
    
    return vkShaderStageFlags;
}

constexpr vk::ImageUsageFlags toVkImageUsageFlags(TextureUsages use)
{
    vk::ImageUsageFlags vkUsages;
    
    if (use & TextureUsage::shaderRead)
        vkUsages |= vk::ImageUsageFlagBits::eSampled;
    if (use & TextureUsage::colorAttachment)
        vkUsages |= vk::ImageUsageFlagBits::eColorAttachment;
    if (use & TextureUsage::depthStencilAttachment)
        vkUsages |= vk::ImageUsageFlagBits::eDepthStencilAttachment;

    return vkUsages;
}

constexpr vk::ImageAspectFlags toVkImageAspectFlags(TextureUsages use)
{
    vk::ImageAspectFlags vkImgAspectFlags;
    
    if (use & TextureUsage::shaderRead)
        vkImgAspectFlags |= vk::ImageAspectFlagBits::eNone; // ?
    if (use & TextureUsage::colorAttachment)
        vkImgAspectFlags |= vk::ImageAspectFlagBits::eColor;
    if (use & TextureUsage::depthStencilAttachment)
        vkImgAspectFlags |= vk::ImageAspectFlagBits::eDepth;

    return vkImgAspectFlags;
}

}

#endif // VULKANENUMS_HPP
