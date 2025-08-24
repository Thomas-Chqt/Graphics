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
    VulkanParameterBlockPool(VulkanParameterBlockPool&&) noexcept;

    VulkanParameterBlockPool(const VulkanDevice*);

    VulkanParameterBlock& get(const ParameterBlock::Layout&);
    void swapPools(); // swap back and front pools
    void reset(); // reset the back pool
    void clear();

    ~VulkanParameterBlockPool();

private:
    struct PoolData
    {
        vk::DescriptorPool descriptorPool;
        ext::deque<VulkanParameterBlock> usedPBlocks;
    };

    const VulkanDevice* m_device = nullptr;

    ext::array<PoolData, 2> m_pools;

    PoolData* m_frontPool = &m_pools[0];
    PoolData* m_backPool = &m_pools[1];

public:
    VulkanParameterBlockPool& operator=(const VulkanParameterBlockPool&) = delete;
    VulkanParameterBlockPool& operator=(VulkanParameterBlockPool&&) noexcept;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCKPOOL_HPP
