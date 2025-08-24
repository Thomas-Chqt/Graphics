/*
 * ---------------------------------------------------
 * MetalCommandBufferPool.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/18 08:09:58
 * ---------------------------------------------------
 */

#include "Metal/MetalCommandBufferPool.hpp"
#include "Metal/MetalCommandBuffer.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <utility>
    namespace ext = std;
#endif

class MTLCommandQueue;

namespace gfx
{

MetalCommandBufferPool::MetalCommandBufferPool(MetalCommandBufferPool&& other) noexcept
    : m_queue(ext::exchange(other.m_queue, nullptr)),
      m_pools(ext::move(other.m_pools))
{
    other.m_frontPool = &other.m_pools[0];
    other.m_backPool = &other.m_pools[1];
}

MetalCommandBufferPool::MetalCommandBufferPool(const id<MTLCommandQueue>* queue)
    : m_queue(queue)
{
}

MetalCommandBuffer& MetalCommandBufferPool::get()
{
    MetalCommandBuffer b = MetalCommandBuffer(*m_queue);
    m_frontPool->commandBuffers.push_back(ext::move(b));
    return m_frontPool->commandBuffers.back();
}

void MetalCommandBufferPool::swapPools()
{
    ext::swap(m_frontPool, m_backPool);
}

void MetalCommandBufferPool::reset()
{
    m_backPool->commandBuffers.clear();
}

void MetalCommandBufferPool::clear()
{
    m_frontPool = &m_pools[0];
    m_backPool = &m_pools[1];
    for (auto& pool : m_pools)
        pool.commandBuffers.clear();
    m_queue = nullptr;
}

MetalCommandBufferPool::~MetalCommandBufferPool()
{
    clear();
}

MetalCommandBufferPool& MetalCommandBufferPool::operator=(MetalCommandBufferPool&& other) noexcept
{
    if (this != &other)
    {
        clear();

        m_queue = ext::exchange(other.m_queue, nullptr);
        m_pools = ext::move(other.m_pools);

        other.m_frontPool = &other.m_pools[0];
        other.m_backPool = &other.m_pools[1];
    }
    return *this;
}

}
