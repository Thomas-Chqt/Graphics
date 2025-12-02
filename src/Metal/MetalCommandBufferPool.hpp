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

#include "Graphics/CommandBufferPool.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Metal/MetalCommandBuffer.hpp"

namespace gfx
{

class MetalCommandBufferPool : public CommandBufferPool
{
public:
    MetalCommandBufferPool() = default;
    MetalCommandBufferPool(const MetalCommandBufferPool&) = delete;
    MetalCommandBufferPool(MetalCommandBufferPool&&) = delete;

    MetalCommandBufferPool(const id<MTLCommandQueue>*);

    std::shared_ptr<CommandBuffer> get() override;
    void reset() override;

    ~MetalCommandBufferPool() override = default;

private:
    const id<MTLCommandQueue>* m_queue = nullptr;

    std::deque<std::shared_ptr<MetalCommandBuffer>> m_availableCommandBuffers;
    std::deque<std::shared_ptr<MetalCommandBuffer>> m_usedCommandBuffers;

public:
    MetalCommandBufferPool& operator = (const MetalCommandBufferPool&) = delete;
    MetalCommandBufferPool& operator = (MetalCommandBufferPool&&) = delete;
};

}

#endif // METALCOMMANDBUFFERPOOL_HPP
