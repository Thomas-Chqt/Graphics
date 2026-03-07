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
#include "Graphics/Texture.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanSampler.hpp"

#include <bit>
#include <cassert>
#include <memory>

#if defined (GFX_IMGUI_ENABLED)
#include "imgui_impl_vulkan.h"
#endif

namespace gfx
{

VulkanTexture::VulkanTexture(const VulkanDevice* device, vk::Image&& vkImage, const Texture::Descriptor& desc)
    : m_device(device),
      m_width(desc.width), m_height(desc.height),
      m_type(desc.type),
      m_pixelFormat(desc.pixelFormat),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode),
      m_vkImage(std::move(vkImage))
{
    m_subresourceRange = vk::ImageSubresourceRange{}
          .setAspectMask(toVkImageAspectFlags(desc.usages))
          .setBaseMipLevel(0)
          .setLevelCount(1)
          .setBaseArrayLayer(0)
          .setLayerCount(desc.type == TextureType::textureCube ? 6 : 1);

    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setImage(m_vkImage)
        .setViewType(desc.type == TextureType::textureCube ? vk::ImageViewType::eCube : vk::ImageViewType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setComponents(vk::ComponentMapping{}
            .setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity))
        .setSubresourceRange(m_subresourceRange);

    m_vkImageView = m_device->vkDevice().createImageView(imageViewCreateInfo);
}

VulkanTexture::VulkanTexture(const VulkanDevice* device, const Texture::Descriptor& desc)
    : m_device(device),
      m_width(desc.width), m_height(desc.height),
      m_type(desc.type),
      m_pixelFormat(desc.pixelFormat),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode)
{
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
        .setMipLevels(1)
        .setArrayLayers(desc.type == TextureType::textureCube ? 6 : 1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage(toVkImageUsageFlags(desc.usages))
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);

    VkImage image = VK_NULL_HANDLE;
    vmaCreateImage(m_device->allocator(), &imageCreateInfo, &allocationCreateInfo, &image, &m_allocation, &m_allocInfo);
    m_vkImage = std::exchange(image, VK_NULL_HANDLE);

    m_subresourceRange = vk::ImageSubresourceRange{}
        .setAspectMask(toVkImageAspectFlags(desc.usages))
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(desc.type == TextureType::textureCube ? 6 : 1);

    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setImage(m_vkImage)
        .setViewType(desc.type == TextureType::textureCube ? vk::ImageViewType::eCube : vk::ImageViewType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setComponents(vk::ComponentMapping{}
            .setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity))
        .setSubresourceRange(m_subresourceRange);

    m_vkImageView = m_device->vkDevice().createImageView(imageViewCreateInfo);
}

#if defined (GFX_IMGUI_ENABLED)
void VulkanTexture::initImTextureId(const std::shared_ptr<Sampler>& sampler)
{
    auto vulkanSampler = std::dynamic_pointer_cast<VulkanSampler>(sampler);
    assert(vulkanSampler);

    m_imTextureIdSampler = vulkanSampler;
    if (m_imTextureId.has_value())
        ImGui_ImplVulkan_RemoveTexture(std::bit_cast<VkDescriptorSet>(*m_imTextureId));
    m_imTextureId = std::bit_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(m_imTextureIdSampler->vkSampler(), m_vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
}
#endif

VulkanTexture::~VulkanTexture()
{
    if (m_imTextureId.has_value())
        ImGui_ImplVulkan_RemoveTexture(std::bit_cast<VkDescriptorSet>(*m_imTextureId));
    m_device->vkDevice().destroyImageView(m_vkImageView);
    if (m_allocation != VK_NULL_HANDLE)
        vmaDestroyImage(m_device->allocator(), m_vkImage, m_allocation);
}

} // namespace gfx
