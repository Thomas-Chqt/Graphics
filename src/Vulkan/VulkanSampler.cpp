/*
 * ---------------------------------------------------
 * VulkanSampler.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/15 23:06:30
 * ---------------------------------------------------
 */

#include "Graphics/Sampler.hpp"

#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

VulkanSampler::VulkanSampler(const VulkanDevice* device, const Sampler::Descriptor& desc)
    : m_device(device)
{
    auto samplerCreateInfo = vk::SamplerCreateInfo{}
        .setMagFilter(toVkFilter(desc.magFilter))
        .setMinFilter(toVkFilter(desc.minFilter))
        .setAddressModeU(toVkSamplerAddressMode(desc.sAddressMode))
        .setAddressModeV(toVkSamplerAddressMode(desc.tAddressMode))
        .setAddressModeW(toVkSamplerAddressMode(desc.rAddressMode))
        .setMipLodBias(0.0f)
        .setAnisotropyEnable(vk::False)
        .setMaxAnisotropy(1)
        .setCompareEnable(vk::False)
        .setCompareOp(vk::CompareOp::eNever)
        .setMinLod(0.0f)
        .setMaxLod(0.0f)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(vk::False);

    m_vkSampler = m_device->vkDevice().createSampler(samplerCreateInfo);
}

VulkanSampler::~VulkanSampler()
{
    m_device->vkDevice().destroySampler(m_vkSampler);
}

}
