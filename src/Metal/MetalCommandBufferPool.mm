/*
 * ---------------------------------------------------
 * MetalCommandBufferPool.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/18 08:09:58
 * ---------------------------------------------------
 */

#include "Graphics/CommandBuffer.hpp"

#include "Metal/MetalCommandBufferPool.hpp"
#include "Metal/MetalCommandBuffer.hpp"

namespace gfx
{

MetalCommandBufferPool::MetalCommandBufferPool(const id<MTLCommandQueue>* queue)
    : m_queue(queue)
{
}

std::unique_ptr<CommandBuffer> MetalCommandBufferPool::get()
{
    std::unique_ptr<MetalCommandBuffer> commandBuffer;
    if (m_availableCommandBuffers.empty() == false) {
        commandBuffer = std::move(m_availableCommandBuffers.front());
        m_availableCommandBuffers.pop_front();
    }
    else {
        commandBuffer = std::make_unique<MetalCommandBuffer>();
    }
    assert(m_usedCommandBuffers.contains(commandBuffer.get()) == false);
    *commandBuffer = MetalCommandBuffer(*m_queue, this); // in metal; the command buffer need to be destoyed and recreated every frame, so availabe command buffer are empty
    m_usedCommandBuffers.insert(commandBuffer.get());
    return commandBuffer;
}

void MetalCommandBufferPool::release(std::unique_ptr<CommandBuffer>&& aCommandBuffer) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    auto* commandBuffer = dynamic_cast<MetalCommandBuffer*>(aCommandBuffer.release());
    assert(commandBuffer);
    *commandBuffer = MetalCommandBuffer(); // destructor will remove the buffer from m_usedCommandBuffers
    assert(m_usedCommandBuffers.contains(commandBuffer) == false);
    m_availableCommandBuffers.push_back(std::unique_ptr<MetalCommandBuffer>(commandBuffer));
}

void MetalCommandBufferPool::release(CommandBuffer* aCommandBuffer)
{
    auto* commandBuffer = dynamic_cast<MetalCommandBuffer*>(aCommandBuffer);
    assert(commandBuffer);
    assert(m_usedCommandBuffers.contains(commandBuffer));
    m_usedCommandBuffers.erase(commandBuffer);
}

MetalCommandBufferPool::~MetalCommandBufferPool()
{
    for (auto& cmdBuffer : m_usedCommandBuffers) {
        // commandBuffers can outlive the pool, so clearing the pool
        // prevent from trying to release a buffer to a freed pool
        cmdBuffer->clearSourcePool();
    }
}

}
