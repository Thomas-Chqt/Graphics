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

namespace gfx
{

class VulkanDevice;
class VulkanParameterBlockPool;

class VulkanParameterBlock : public ParameterBlock
{
public:
    VulkanParameterBlock() = delete;
    VulkanParameterBlock(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock(VulkanParameterBlock&&) = delete;

    VulkanParameterBlock(const VulkanDevice*, const ext::shared_ptr<vk::DescriptorPool>&, const ParameterBlock::Layout&, VulkanParameterBlockPool*);

    void setBinding(uint32_t idx, const ext::shared_ptr<Buffer>&) override;
    void setBinding(uint32_t idx, const ext::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, const ext::shared_ptr<Sampler>&) override;

    inline const ParameterBlock::Layout& layout() const { return m_layout; }
    inline void clearSourcePool() { m_sourcePool = nullptr; }

    inline const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

    inline const ext::map<ext::shared_ptr<VulkanBuffer>, ParameterBlock::Binding>& usedBuffers() const { return m_usedBuffers; }
    inline const ext::map<ext::shared_ptr<VulkanTexture>, ParameterBlock::Binding>& usedTextures() const { return m_usedTextures; }
    inline const ext::map<ext::shared_ptr<VulkanSampler>, ParameterBlock::Binding>& usedSamplers() const { return m_usedSampler; }

    ~VulkanParameterBlock() override; 

private:
    const VulkanDevice* m_device;
    ext::shared_ptr<vk::DescriptorPool> m_descriptorPool;
    ParameterBlock::Layout m_layout;
    VulkanParameterBlockPool* m_sourcePool;

    vk::DescriptorSet m_descriptorSet;

    ext::map<ext::shared_ptr<VulkanBuffer>, ParameterBlock::Binding> m_usedBuffers;
    ext::map<ext::shared_ptr<VulkanTexture>, ParameterBlock::Binding> m_usedTextures;
    ext::map<ext::shared_ptr<VulkanSampler>, ParameterBlock::Binding> m_usedSampler;

public:
    VulkanParameterBlock& operator=(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock& operator=(VulkanParameterBlock&&) = delete;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCK_HPP
