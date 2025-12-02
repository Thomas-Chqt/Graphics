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

std::shared_ptr<CommandBuffer> MetalCommandBufferPool::get()
{
    std::shared_ptr<MetalCommandBuffer> commandBuffer;
    if (m_availableCommandBuffers.empty() == false) {
        commandBuffer = std::move(m_availableCommandBuffers.front());
        m_availableCommandBuffers.pop_front();
        *commandBuffer = MetalCommandBuffer(*m_queue); // in metal; the command buffer need to be destoyed and recreated every frame, so availabe command buffer are empty
    }
    else {
        commandBuffer = std::make_shared<MetalCommandBuffer>(*m_queue);
    }
    m_usedCommandBuffers.push_back(commandBuffer);
    return commandBuffer;
}

void MetalCommandBufferPool::reset()
{
    for (auto& commandBuffer : m_usedCommandBuffers) {
        *commandBuffer = MetalCommandBuffer();
        m_availableCommandBuffers.push_back(std::move(commandBuffer));
    }
    m_usedCommandBuffers.clear();
}

}
