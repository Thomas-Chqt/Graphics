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

#include "Vulkan/VulkanDevice.hpp"

#include <Vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
#endif

namespace gfx
{

class VulkanTexture : public Texture
{
public:
    VulkanTexture() = delete;
    VulkanTexture(const VulkanTexture&) = delete;
    VulkanTexture(VulkanTexture&&) = delete;

    VulkanTexture(vk::Image, const Texture::Descriptor&);

    inline PixelFormat pixelFormat(void) const override { return m_pixelFormat; };

    inline const vk::Image vkImage() const { return m_vkImage; }

    ~VulkanTexture();

private:
    bool m_shouldDestroyImage;
    
    VulkanDevice* m_device = nullptr;
    vk::Image m_vkImage;

    PixelFormat m_pixelFormat;

public:
    VulkanTexture& operator=(const VulkanTexture&) = delete;
    VulkanTexture& operator=(VulkanTexture&&) = delete;
};

} // namespace gfx

#endif // VULKANTEXTURE_HPP
