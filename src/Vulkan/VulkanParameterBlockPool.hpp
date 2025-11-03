/*
 * ---------------------------------------------------
 * VulkanParameterBlockPool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 17:43:12
 * ---------------------------------------------------
 */

#ifndef VULKANPARAMETERBLOCKPOOL_HPP
#define VULKANPARAMETERBLOCKPOOL_HPP

#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanParameterBlock.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanParameterBlockPool : public ParameterBlockPool
{
public:
    VulkanParameterBlockPool() = default;
    VulkanParameterBlockPool(const VulkanParameterBlockPool&) = delete;
    VulkanParameterBlockPool(VulkanParameterBlockPool&&) = delete;

    VulkanParameterBlockPool(const VulkanDevice*, const ParameterBlockPool::Descriptor&);

    std::unique_ptr<ParameterBlock> get(const ParameterBlock::Layout&) override;
    void release(ParameterBlock*);

    ~VulkanParameterBlockPool() override;

private:
    const VulkanDevice* m_device = nullptr;

    std::shared_ptr<vk::DescriptorPool> m_descriptorPool; // blocks can outlive the pool, only the vk::DescriptorPool need to remain alive

    std::set<VulkanParameterBlock*> m_usedParameterBlocks;

public:
    VulkanParameterBlockPool& operator=(const VulkanParameterBlockPool&) = delete;
    VulkanParameterBlockPool& operator=(VulkanParameterBlockPool&&) = delete;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCKPOOL_HPP
