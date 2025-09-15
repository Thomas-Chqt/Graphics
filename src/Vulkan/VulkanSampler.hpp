/*
 * ---------------------------------------------------
 * VulkanSampler.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/15 21:12:27
 * ---------------------------------------------------
 */

#ifndef VULKANSAMPLER_HPP
#define VULKANSAMPLER_HPP

#include "Graphics/Sampler.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    namespace ext = std; // NOLINT
#endif

namespace gfx
{

class VulkanDevice;

class VulkanSampler : public Sampler
{
public:
    VulkanSampler() = delete;
    VulkanSampler(const VulkanSampler&) = delete;
    VulkanSampler(VulkanSampler&&) = delete;

    VulkanSampler(const VulkanDevice*, const Sampler::Descriptor&);

    inline const vk::Sampler& vkSampler() const { return m_vkSampler; }

    ~VulkanSampler() override;

private:
    const VulkanDevice* m_device;
    vk::Sampler m_vkSampler;

public:
    VulkanSampler& operator=(const VulkanSampler&) = delete;
    VulkanSampler& operator=(VulkanSampler&&) = delete;
};

} // namespace gfx

#endif // VULKANSAMPLER_HPP
