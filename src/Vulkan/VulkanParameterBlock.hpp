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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstdint>
    #include <memory>
    #include <map>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanParameterBlock : public ParameterBlock
{
public:
    VulkanParameterBlock() = delete;
    VulkanParameterBlock(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock(VulkanParameterBlock&&) = default;

    VulkanParameterBlock(const VulkanDevice*, vk::DescriptorSet&& descriptorSet, const ParameterBlock::Layout&);

    void setBinding(uint32_t idx, const ext::shared_ptr<Buffer>&) override;

    inline const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }
    inline const ext::map<ParameterBlock::Binding, ext::shared_ptr<VulkanBuffer>> usedBuffers() const { return m_usedBuffers; }

    ~VulkanParameterBlock() override = default;

private:
    const VulkanDevice* m_device;
    vk::DescriptorSet m_descriptorSet;
    ext::vector<ParameterBlock::Binding> m_bindings;

    ext::map<ParameterBlock::Binding, ext::shared_ptr<VulkanBuffer>> m_usedBuffers;
public:
    VulkanParameterBlock& operator=(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock& operator=(VulkanParameterBlock&&) = default;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCK_HPP
