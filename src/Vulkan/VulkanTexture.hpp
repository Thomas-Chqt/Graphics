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

class VulkanTexture
{
public:
    VulkanTexture() = delete;
    VulkanTexture(const VulkanTexture&) = delete;
    VulkanTexture(VulkanTexture&&) = delete;

    VulkanTexture(const VulkanDevice*, vk::Image&&, const Texture::Descriptor&);
    VulkanTexture(const VulkanDevice*, const Texture::Descriptor&);

    TextureType type() const;

    uint32_t width() const;
    uint32_t height() const;

    uint32_t mipLevelCount() const;
    uint32_t arrayLayerCount() const;

    PixelFormat pixelFormat() const;

    TextureUsages usages() const;
    ResourceStorageMode storageMode() const;

    const vk::Image& vkImage() const;

    ImageSyncState& syncState();

    virtual ~VulkanTexture();

protected:
    const VulkanDevice* m_device = nullptr;

private:
    TextureType m_type;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_mipLevelCount;
    uint32_t m_arrayLayerCount;
    PixelFormat m_pixelFormat;
    TextureUsages m_usages;
    ResourceStorageMode m_storageMode;

    VmaAllocation m_allocation = VK_NULL_HANDLE;
    vk::Image m_vkImage;

    ImageSyncState m_syncState;

public:
    VulkanTexture& operator=(const VulkanTexture&) = delete;
    VulkanTexture& operator=(VulkanTexture&&) = delete;
};

} // namespace gfx

#endif // VULKANTEXTURE_HPP
