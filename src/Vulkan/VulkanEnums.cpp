/*
 * ---------------------------------------------------
 * VulkanEnums.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:26:34
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"

#include "Vulkan/VulkanEnums.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

vk::Format toVkFormat(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::BGRA8Unorm:
        return vk::Format::eB8G8R8A8Unorm;
    case PixelFormat::BGRA8Unorm_sRGB:
        return vk::Format::eB8G8R8A8Srgb;
    default:
        throw ext::runtime_error("not implemented");
    }
}

PixelFormat toPixelFormat(vk::Format fmt)
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

vk::ColorSpaceKHR toVkColorSpaceKHR(PixelFormat pxf)
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

vk::PresentModeKHR toVkPresentModeKHR(PresentMode pmd)
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

PresentMode toPresentMode(vk::PresentModeKHR pmd)
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

vk::AttachmentLoadOp toVkAttachmentLoadOp(LoadAction loa)
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

vk::Format toVkFormat(VertexAttributeFormat fmt)
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

vk::BufferUsageFlags toVkBufferUsageFlags(BufferUsages use)
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

}
