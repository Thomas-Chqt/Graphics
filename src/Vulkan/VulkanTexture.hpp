/*
 * ---------------------------------------------------
 * VulkanTexture.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:23:52
 * ---------------------------------------------------
 */

#ifndef VULKANTEXTURE_HPP
#define VULKANTEXTURE_HPP

#include "Graphics/Texture.hpp"
#include "Graphics/Enums.hpp"

#include "Vulkan/Sync.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanTexture : public Texture
{
public:
    VulkanTexture() = delete;
    VulkanTexture(const VulkanTexture&) = delete;
    VulkanTexture(VulkanTexture&&) = delete;

    VulkanTexture(const VulkanDevice*, vk::Image&&, const Texture::Descriptor&);
    VulkanTexture(const VulkanDevice*, const Texture::Descriptor&);

    inline TextureType type() const override { return m_type; };
    inline uint32_t width() const override { return m_width; }
    inline uint32_t height() const override { return m_height; }
    inline PixelFormat pixelFormat() const override { return m_pixelFormat; };
    inline TextureUsages usages() const override { return m_usages; };
    inline ResourceStorageMode storageMode() const override { return m_storageMode; };

    inline const vk::Image& vkImage() const { return m_vkImage; }

    inline const vk::ImageSubresourceRange& subresourceRange() const { return m_subresourceRange; }
    inline const vk::ImageView& vkImageView() const { return m_vkImageView; }

    inline ImageSyncState& syncState() { return m_syncState; }

    ~VulkanTexture() override;

protected:
    const VulkanDevice* m_device = nullptr;
    uint32_t m_width, m_height;
    TextureType m_type;
    PixelFormat m_pixelFormat;
    TextureUsages m_usages;
    ResourceStorageMode m_storageMode;

    VmaAllocation m_allocation = VK_NULL_HANDLE;
    VmaAllocationInfo m_allocInfo = {};
    vk::Image m_vkImage;

    vk::ImageSubresourceRange m_subresourceRange;
    vk::ImageView m_vkImageView;

    ImageSyncState m_syncState;

public:
    VulkanTexture& operator=(const VulkanTexture&) = delete;
    VulkanTexture& operator=(VulkanTexture&&) = delete;
};

} // namespace gfx

#endif // VULKANTEXTURE_HPP
