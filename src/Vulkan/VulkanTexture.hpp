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

#include "common.hpp"
#include "Vulkan/Sync.hpp"
#include "Vulkan/vk_mem_alloc.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstdint>
    namespace ext = std; // NOLINT
#endif

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

    inline uint32_t width() const override { return m_width; }
    inline uint32_t height() const override { return m_height; }
    inline PixelFormat pixelFormat() const override { return m_pixelFormat; };
    inline TextureUsages usages() const override { return m_usages; };
    inline ResourceStorageMode storageMode() const override { return m_storageMode; };

    inline const vk::Image& vkImage() const { return currentFrameData().vkImage; }
    inline const vk::ImageView& vkImageView() const { return currentFrameData().vkImageView; }
    inline ImageSyncState& syncState() { return currentFrameData().syncState; }

    inline const vk::ImageSubresourceRange& subresourceRange() const { return m_subresourceRange; }

    ~VulkanTexture() override;

protected:
    struct FrameData
    {
        vk::Image vkImage;
        VmaAllocation allocation;
        VmaAllocationInfo allocInfo;
        vk::ImageView vkImageView;
        ImageSyncState syncState;
    };

    FrameData& currentFrameData();
    const FrameData& currentFrameData() const;

    const VulkanDevice* m_device = nullptr;
    uint32_t m_width, m_height;
    PixelFormat m_pixelFormat;
    TextureUsages m_usages;
    ResourceStorageMode m_storageMode;

    bool m_shouldDestroyImg;
    vk::ImageSubresourceRange m_subresourceRange;

    PerFrameInFlight<FrameData> m_frameDatas;
    uint8_t m_imageCount;

public:
    VulkanTexture& operator=(const VulkanTexture&) = delete;
    VulkanTexture& operator=(VulkanTexture&&) = delete;
};

} // namespace gfx

#endif // VULKANTEXTURE_HPP
