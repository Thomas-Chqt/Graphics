/*
 * ---------------------------------------------------
 * VulkanParameterBlock.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 10:51:12
 * ---------------------------------------------------
 */

#ifndef VULKANPARAMETERBLOCK_HPP
#define VULKANPARAMETERBLOCK_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanParameterBlockLayout.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanParameterBlock : public ParameterBlock
{
public:
    VulkanParameterBlock() = default;
    VulkanParameterBlock(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock(VulkanParameterBlock&&) = delete;

    VulkanParameterBlock(const VulkanDevice*, const std::shared_ptr<VulkanParameterBlockLayout>&, const std::shared_ptr<vk::DescriptorPool>&);

    inline std::shared_ptr<ParameterBlockLayout> layout() const override { return m_layout; }

    void setBinding(uint32_t idx, const std::shared_ptr<Buffer>&) override;
    void setBinding(uint32_t idx, const std::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, const std::shared_ptr<Sampler>&) override;

    inline const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

    inline const std::map<std::shared_ptr<VulkanBuffer>, ParameterBlockBinding>& usedBuffers() const { return m_usedBuffers; }
    inline const std::map<std::shared_ptr<VulkanTexture>, ParameterBlockBinding>& usedTextures() const { return m_usedTextures; }
    inline const std::map<std::shared_ptr<VulkanSampler>, ParameterBlockBinding>& usedSamplers() const { return m_usedSampler; }

    ~VulkanParameterBlock() override = default;

private:
    const VulkanDevice* m_device = nullptr;
    std::shared_ptr<VulkanParameterBlockLayout> m_layout;
    std::shared_ptr<vk::DescriptorPool> m_descriptorPool;

    vk::DescriptorSet m_descriptorSet;

    std::map<std::shared_ptr<VulkanBuffer>, ParameterBlockBinding> m_usedBuffers;
    std::map<std::shared_ptr<VulkanTexture>, ParameterBlockBinding> m_usedTextures;
    std::map<std::shared_ptr<VulkanSampler>, ParameterBlockBinding> m_usedSampler;

public:
    VulkanParameterBlock& operator=(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock& operator=(VulkanParameterBlock&&) = default;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCK_HPP
