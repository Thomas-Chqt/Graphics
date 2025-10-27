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

    std::unique_ptr<CommandBuffer> get() override;
    void release(std::unique_ptr<CommandBuffer>&&) override;
    void release(CommandBuffer*);

    ~MetalCommandBufferPool() override;

private:
    const id<MTLCommandQueue>* m_queue = nullptr;

    std::deque<std::unique_ptr<MetalCommandBuffer>> m_availableCommandBuffers;
    std::set<MetalCommandBuffer*> m_usedCommandBuffers;

public:
    MetalCommandBufferPool& operator = (const MetalCommandBufferPool&) = delete;
    MetalCommandBufferPool& operator = (MetalCommandBufferPool&&) = delete;
};

}

#endif // METALCOMMANDBUFFERPOOL_HPP
