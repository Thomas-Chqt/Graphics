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

#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanParameterBlock.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <deque>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanParameterBlockPool
{
public:
    VulkanParameterBlockPool() = default;
    VulkanParameterBlockPool(const VulkanParameterBlockPool&) = delete;
    VulkanParameterBlockPool(VulkanParameterBlockPool&&) = default;

    VulkanParameterBlockPool(const VulkanDevice*);

    VulkanParameterBlock& get(const ParameterBlock::Layout&);
    void reset();

    ~VulkanParameterBlockPool();

private:
    const VulkanDevice* m_device = nullptr;
    vk::DescriptorPool m_descriptorPool;
    ext::deque<VulkanParameterBlock> m_usedPBlocks;

public:
    VulkanParameterBlockPool& operator=(const VulkanParameterBlockPool&) = delete;
    VulkanParameterBlockPool& operator=(VulkanParameterBlockPool&&) noexcept;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCKPOOL_HPP
