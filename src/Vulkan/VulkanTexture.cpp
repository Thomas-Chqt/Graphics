/*
 * ---------------------------------------------------
 * VulkanTexture.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:27:52
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanTexture.hpp"

#include "Graphics/Texture.hpp"
#include "Graphics/Enums.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"

namespace gfx
{

VulkanTexture::VulkanTexture(const VulkanDevice* device, vk::Image&& vkImage, const Texture::Descriptor& desc)
    : m_device(device)
    , m_type(desc.type)
    , m_width(desc.width)
    , m_height(desc.height)
    , m_mipLevelCount(desc.mipLevelCount)
    , m_arrayLayerCount(desc.arrayLayerCount)
    , m_pixelFormat(desc.pixelFormat)
    , m_usages(desc.usages)
    , m_storageMode(desc.storageMode)
    , m_vkImage(std::move(vkImage))
{
    assert(desc.width > 0 && desc.height > 0);
    assert(desc.mipLevelCount > 0);
    assert(desc.arrayLayerCount > 0);
    assert(desc.type == TextureType::texture2dArray || desc.arrayLayerCount == 1);
}

VulkanTexture::VulkanTexture(const VulkanDevice* device, const Texture::Descriptor& desc)
    : m_device(device)
    , m_type(desc.type)
    , m_width(desc.width)
    , m_height(desc.height)
    , m_mipLevelCount(desc.mipLevelCount)
    , m_arrayLayerCount(desc.arrayLayerCount)
    , m_pixelFormat(desc.pixelFormat)
    , m_usages(desc.usages)
    , m_storageMode(desc.storageMode)
{
    assert(desc.width > 0 && desc.height > 0);
    assert(desc.mipLevelCount > 0);
    assert(desc.arrayLayerCount > 0);
    assert(desc.type == TextureType::texture2dArray || desc.arrayLayerCount == 1);

    VmaAllocationCreateInfo allocationCreateInfo = { .usage = VMA_MEMORY_USAGE_AUTO, };
    if (desc.storageMode == ResourceStorageMode::hostVisible)
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo{}
        .setFlags(desc.type == TextureType::textureCube ? vk::ImageCreateFlagBits::eCubeCompatible : vk::ImageCreateFlags{})
        .setImageType(vk::ImageType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setExtent(vk::Extent3D{}
            .setWidth(desc.width)
            .setHeight(desc.height)
            .setDepth(1))
        .setMipLevels(desc.mipLevelCount)
        .setArrayLayers(desc.type == TextureType::textureCube ? 6u : desc.arrayLayerCount)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage(toVkImageUsageFlags(desc.usages))
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);

    VkImage image = VK_NULL_HANDLE;
    VmaAllocationInfo allocInfo;
    vmaCreateImage(m_device->allocator(), &imageCreateInfo, &allocationCreateInfo, &image, &m_allocation, &allocInfo);
    m_vkImage = std::exchange(image, VK_NULL_HANDLE);
}

TextureType VulkanTexture::type() const
{
    return m_type;
}

uint32_t VulkanTexture::width() const
{
    return m_width;
}

uint32_t VulkanTexture::height() const
{
    return m_height;
}

uint32_t VulkanTexture::mipLevelCount() const
{
    return m_mipLevelCount;
}

uint32_t VulkanTexture::arrayLayerCount() const
{
    return m_arrayLayerCount;
}

PixelFormat VulkanTexture::pixelFormat() const
{
    return m_pixelFormat;
}

TextureUsages VulkanTexture::usages() const
{
    return m_usages;
}

ResourceStorageMode VulkanTexture::storageMode() const
{
    return m_storageMode;
}

const vk::Image& VulkanTexture::vkImage() const
{
    return m_vkImage;
}

ImageSyncState& VulkanTexture::syncState()
{
    return m_syncState;
}

VulkanTexture::~VulkanTexture()
{
    if (m_allocation != VK_NULL_HANDLE)
        vmaDestroyImage(m_device->allocator(), m_vkImage, m_allocation);
}

} // namespace gfx
