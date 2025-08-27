/*
 * ---------------------------------------------------
 * MetalCommandBufferPool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/18 08:05:26
 * ---------------------------------------------------
 */

#ifndef METALCOMMANDBUFFERPOOL_HPP
#define METALCOMMANDBUFFERPOOL_HPP

#include "Metal/MetalCommandBuffer.hpp"

namespace gfx
{

class MetalCommandBufferPool
{
public:
    MetalCommandBufferPool() = default;
    MetalCommandBufferPool(const MetalCommandBufferPool&) = delete;
    MetalCommandBufferPool(MetalCommandBufferPool&&) noexcept;

    MetalCommandBufferPool(const id<MTLCommandQueue>*);
    
    MetalCommandBuffer& get(); // get from the front pool
    void swapPools(); // swap back and front pools
    void reset(); // reset the back pool
    void clear();

    ~MetalCommandBufferPool();

private:
    struct PoolData
    {
        ext::deque<MetalCommandBuffer> commandBuffers;
    };

    const id<MTLCommandQueue>* m_queue = nullptr;

    ext::array<PoolData, 2> m_pools;

    PoolData* m_frontPool = &m_pools[0];
    PoolData* m_backPool = &m_pools[1];
    
public:
    MetalCommandBufferPool& operator = (const MetalCommandBufferPool&) = delete;
    MetalCommandBufferPool& operator = (MetalCommandBufferPool&&) noexcept;
};

}

#endif // METALCOMMANDBUFFERPOOL_HPP
