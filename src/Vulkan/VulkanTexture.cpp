/*
 * ---------------------------------------------------
 * VulkanTexture.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:27:52
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanTexture.hpp"
#include "Graphics/Enums.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/vk_mem_alloc.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <utility>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

VulkanTexture::VulkanTexture(const VulkanDevice* device, vk::Image&& vkImage, const Texture::Descriptor& desc)
    : m_device(device),
      m_width(desc.width), m_height(desc.height),
      m_pixelFormat(desc.pixelFormat),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode),
      m_shouldDestroyImg(false),
      m_subresourceRange(vk::ImageSubresourceRange{}
          .setAspectMask(toVkImageAspectFlags(desc.usages))
          .setBaseMipLevel(0)
          .setLevelCount(1)
          .setBaseArrayLayer(0)
          .setLayerCount(1)),
      m_imageCount(1)
{
    m_frameDatas.front().vkImage = ext::move(vkImage);

    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setImage(m_frameDatas.front().vkImage)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setComponents(vk::ComponentMapping{}
            .setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity))
        .setSubresourceRange(m_subresourceRange);

    m_frameDatas.front().vkImageView = m_device->vkDevice().createImageView(imageViewCreateInfo);
}

VulkanTexture::VulkanTexture(const VulkanDevice* device, const Texture::Descriptor& desc)
    : m_device(device),
      m_width(desc.width), m_height(desc.height),
      m_pixelFormat(desc.pixelFormat),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode),
      m_shouldDestroyImg(true),
      m_subresourceRange(vk::ImageSubresourceRange{}
          .setAspectMask(toVkImageAspectFlags(desc.usages))
          .setBaseMipLevel(0)
          .setLevelCount(1)
          .setBaseArrayLayer(0)
          .setLayerCount(1))
{
    if (desc.usages & TextureUsage::depthStencilAttachment ||
        desc.storageMode == ResourceStorageMode::hostVisible) {
        m_imageCount = 3;
    } else {
        m_imageCount = 1;
    }

    VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo{}
        .setImageType(vk::ImageType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setExtent(vk::Extent3D{}
            .setWidth(desc.width)
            .setHeight(desc.height)
            .setDepth(1))
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage(toVkImageUsageFlags(desc.usages))
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);

    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setComponents(vk::ComponentMapping{}
            .setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity))
        .setSubresourceRange(m_subresourceRange);

    VmaAllocationCreateInfo allocationCreateInfo = { .usage = VMA_MEMORY_USAGE_AUTO, };
    if (desc.storageMode == ResourceStorageMode::hostVisible)
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    for (uint8_t i = 0; i < m_imageCount; i++)
    {
        FrameData& frameData = m_frameDatas.at(i);
        VkImage image = VK_NULL_HANDLE;
        vmaCreateImage(
            m_device->allocator(),
            &imageCreateInfo,
            &allocationCreateInfo,
            &image,
            &frameData.allocation,
            &frameData.allocInfo);
        frameData.vkImage = ext::exchange(image, VK_NULL_HANDLE);
        imageViewCreateInfo.setImage(frameData.vkImage);
        frameData.vkImageView = m_device->vkDevice().createImageView(imageViewCreateInfo);
    }
}

VulkanTexture::~VulkanTexture()
{
    for (uint8_t i = 0; i < m_imageCount; i++)
    {
        FrameData& frameData = m_frameDatas.at(i);
        m_device->vkDevice().destroyImageView(frameData.vkImageView);
        if (m_shouldDestroyImg)
            vmaDestroyImage(m_device->allocator(), frameData.vkImage, frameData.allocation);
    }
}

VulkanTexture::FrameData& VulkanTexture::currentFrameData()
{
    return m_frameDatas.at(m_device->currentFrameIdx() % m_imageCount);
}

const VulkanTexture::FrameData& VulkanTexture::currentFrameData() const
{
    return m_frameDatas.at(m_device->currentFrameIdx() % m_imageCount);
}

} // namespace gfx
