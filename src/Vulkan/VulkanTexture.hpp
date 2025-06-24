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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <cstdint>
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

    VulkanTexture(const VulkanDevice&, vk::Image, const Texture::Descriptor&);

    inline uint32_t width() const override { return m_width; }
    inline uint32_t height() const override { return m_height; }
    inline PixelFormat pixelFormat(void) const override { return m_pixelFormat; };

    inline const vk::Image& vkImage() const { return m_vkImage; }
    inline const vk::ImageView& vkImageView() const { return m_vkImageView; }

    ~VulkanTexture();

protected:
    const VulkanDevice* m_device = nullptr;

    vk::Image m_vkImage;
    vk::ImageView m_vkImageView;
    bool m_shouldDestroyImg;
    
    uint32_t m_width, m_height;
    PixelFormat m_pixelFormat;

public:
    VulkanTexture& operator=(const VulkanTexture&) = delete;
    VulkanTexture& operator=(VulkanTexture&&) = delete;
};

} // namespace gfx

#endif // VULKANTEXTURE_HPP
