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

namespace gfx
{

constexpr vk::ImageViewType toVkImageViewType(TextureType type)
{
    switch (type)
    {
    case TextureType::texture2d:
        return vk::ImageViewType::e2D;
    case TextureType::texture2dArray:
        return vk::ImageViewType::e2DArray;
    case TextureType::textureCube:
        return vk::ImageViewType::eCube;
    default:
        throw std::runtime_error("not implemented");
    }
}

constexpr vk::Format toVkFormat(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::RGBA8_unorm:
        return vk::Format::eR8G8B8A8Unorm;
    case PixelFormat::RGBA8_sRGB:
        return vk::Format::eR8G8B8A8Srgb;
    case PixelFormat::BGRA8_unorm:
        return vk::Format::eB8G8R8A8Unorm;
    case PixelFormat::BGRA8_sRGB:
        return vk::Format::eB8G8R8A8Srgb;
    case PixelFormat::RGBA16_float:
        return vk::Format::eR16G16B16A16Sfloat;
    case PixelFormat::RGBA32_float:
        return vk::Format::eR32G32B32A32Sfloat;
    case PixelFormat::RG16_float:
        return vk::Format::eR16G16Sfloat;
    case PixelFormat::RG32Uint:
        return vk::Format::eR32G32Uint;
    case PixelFormat::RG32Float:
        return vk::Format::eR32G32Sfloat;
    case PixelFormat::Depth32Float:
        return vk::Format::eD32Sfloat;
    }
    std::unreachable();
}

constexpr std::optional<PixelFormat> toOptPixelFormat(vk::Format fmt)
{
    switch (fmt)
    {
    case vk::Format::eR8G8B8A8Unorm:
        return PixelFormat::RGBA8_unorm;
    case vk::Format::eR8G8B8A8Srgb:
        return PixelFormat::RGBA8_sRGB;
    case vk::Format::eB8G8R8A8Unorm:
        return PixelFormat::BGRA8_unorm;
    case vk::Format::eB8G8R8A8Srgb:
        return PixelFormat::BGRA8_sRGB;
    case vk::Format::eR16G16B16A16Sfloat:
        return PixelFormat::RGBA16_float;
    case vk::Format::eR32G32B32A32Sfloat:
        return PixelFormat::RGBA32_float;
    case vk::Format::eR16G16Sfloat:
        return PixelFormat::RG16_float;
    case vk::Format::eR32G32Uint:
        return PixelFormat::RG32Uint;
    case vk::Format::eR32G32Sfloat:
        return PixelFormat::RG32Float;
    case vk::Format::eD32Sfloat:
        return PixelFormat::Depth32Float;
    default:
        return std::nullopt;
    }
    std::unreachable();
}

constexpr PixelFormat toPixelFormat(vk::Format fmt)
{
    auto opt = toOptPixelFormat(fmt);
    if (opt.has_value() == false)
        throw std::runtime_error("not implemented");
    return *opt;
}

constexpr vk::ColorSpaceKHR toVkColorSpaceKHR(ColorSpace pxf)
{
    switch (pxf)
    {
    case ColorSpace::sRGB_nonLinear:
        return vk::ColorSpaceKHR::eSrgbNonlinear;
    case ColorSpace::displayP3_nonlinear:
        return vk::ColorSpaceKHR::eDisplayP3NonlinearEXT;
    case ColorSpace::displayP3_linear:
        return vk::ColorSpaceKHR::eDisplayP3LinearEXT;
    }
    std::unreachable();
}

constexpr std::optional<ColorSpace> toOptColorSpace(vk::ColorSpaceKHR csp)
{
    switch (csp)
    {
    case vk::ColorSpaceKHR::eSrgbNonlinear:
        return ColorSpace::sRGB_nonLinear;
    case vk::ColorSpaceKHR::eDisplayP3NonlinearEXT:
        return ColorSpace::displayP3_nonlinear;
    case vk::ColorSpaceKHR::eDisplayP3LinearEXT:
        return  ColorSpace::displayP3_linear;
    default:
        return std::nullopt;
    }
    std::unreachable();
}

constexpr ColorSpace toColorSpace(vk::ColorSpaceKHR csp)
{
    auto opt = toOptColorSpace(csp);
    if (opt.has_value() == false)
        throw std::runtime_error("not implemented");
    return *opt;
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
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
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
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
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
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr vk::Format toVkFormat(VertexAttributeFormat fmt)
{
    switch (fmt)
    {
    case VertexAttributeFormat::float2:
        return vk::Format::eR32G32Sfloat;
    case VertexAttributeFormat::float3:
        return vk::Format::eR32G32B32Sfloat;
    case VertexAttributeFormat::float4:
        return vk::Format::eR32G32B32A32Sfloat;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr vk::BufferUsageFlags toVkBufferUsageFlags(BufferUsages use)
{
    vk::BufferUsageFlags vkUsages;

    if (use & BufferUsage::vertexBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eVertexBuffer;
    if (use & BufferUsage::indexBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eIndexBuffer;
    if (use & BufferUsage::constantBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eUniformBuffer;
    if (use & BufferUsage::structuredBuffer)
        vkUsages |= vk::BufferUsageFlagBits::eStorageBuffer;
    if (use & BufferUsage::copySource)
        vkUsages |= vk::BufferUsageFlagBits::eTransferSrc;
    if (use & BufferUsage::copyDestination)
        vkUsages |= vk::BufferUsageFlagBits::eTransferDst;

    return vkUsages;
}

constexpr vk::DescriptorType toVkDescriptorType(BindingType tpe)
{
    switch (tpe)
    {
    case BindingType::constantBuffer:
        return vk::DescriptorType::eUniformBuffer;
    case BindingType::structuredBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case BindingType::sampledTexture:
        return vk::DescriptorType::eSampledImage;
    case BindingType::storageTexture:
        return vk::DescriptorType::eStorageImage;
    case BindingType::sampler:
        return vk::DescriptorType::eSampler;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
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
    if (use & BindingUsage::computeRead)
        vkShaderStageFlags |= vk::ShaderStageFlagBits::eCompute;
    if (use & BindingUsage::computeWrite)
        vkShaderStageFlags |= vk::ShaderStageFlagBits::eCompute;

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
    if (use & TextureUsage::copyDestination)
        vkUsages |= vk::ImageUsageFlagBits::eTransferDst;
    if (use & TextureUsage::copySource)
        vkUsages |= vk::ImageUsageFlagBits::eTransferSrc;
    if (use & TextureUsage::shaderWrite)
        vkUsages |= vk::ImageUsageFlagBits::eStorage;

    return vkUsages;
}

constexpr vk::ImageAspectFlags toVkImageAspectFlags(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::RGBA8_unorm:
    case PixelFormat::RGBA8_sRGB:
    case PixelFormat::BGRA8_unorm:
    case PixelFormat::BGRA8_sRGB:
    case PixelFormat::RGBA16_float:
    case PixelFormat::RGBA32_float:
    case PixelFormat::RG16_float:
    case PixelFormat::RG32Uint:
    case PixelFormat::RG32Float:
        return vk::ImageAspectFlagBits::eColor;
    case PixelFormat::Depth32Float:
        return vk::ImageAspectFlagBits::eDepth;
    default:
        throw std::runtime_error("not implemented");
    }
}

constexpr vk::SamplerAddressMode toVkSamplerAddressMode(SamplerAddressMode mode)
{
    switch (mode)
    {
    case SamplerAddressMode::ClampToEdge:
        return vk::SamplerAddressMode::eClampToEdge;
    case SamplerAddressMode::Repeat:
        return vk::SamplerAddressMode::eRepeat;
    case SamplerAddressMode::MirrorRepeat:
        return vk::SamplerAddressMode::eMirroredRepeat;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr vk::Filter toVkFilter(SamplerMinMagFilter mode)
{
    switch (mode)
    {
    case SamplerMinMagFilter::Nearest:
        return vk::Filter::eNearest;
    case SamplerMinMagFilter::Linear:
        return vk::Filter::eLinear;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr vk::SamplerMipmapMode toVkSamplerMipmapMode(SamplerMipFilter filter)
{
    switch (filter)
    {
    case SamplerMipFilter::NotMipmapped:
    case SamplerMipFilter::Nearest:
        return vk::SamplerMipmapMode::eNearest;
    case SamplerMipFilter::Linear:
        return vk::SamplerMipmapMode::eLinear;
    default:
        throw std::runtime_error("not implemented");
    }
}

constexpr vk::CullModeFlags toVkCullModeFlags(CullMode cullMode)
{
    switch (cullMode)
    {
    case CullMode::none:
        return vk::CullModeFlagBits::eNone;
    case CullMode::front:
        return vk::CullModeFlagBits::eFront;
    case CullMode::back:
        return vk::CullModeFlagBits::eBack;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr vk::CompareOp toVkCompareOp(DepthCompareOperation operation)
{
    switch (operation)
    {
    case DepthCompareOperation::less:
        return vk::CompareOp::eLess;
    case DepthCompareOperation::lessEqual:
        return vk::CompareOp::eLessOrEqual;
    }
    std::unreachable();
}

}

#endif // VULKANENUMS_HPP
