/*
 * ---------------------------------------------------
 * MetalParameterBlockPool.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 19:20:35
 * ---------------------------------------------------
 */

#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalParameterBlockPool.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalDevice.hpp"
#include "MetalParameterBlockLayout.hpp"
#include <memory>

namespace gfx
{

MetalParameterBlockPool::MetalParameterBlockPool(const MetalDevice* device, const ParameterBlockPool::Descriptor& descriptor)
    : m_device(device)
{
    size_t totalBindings = descriptor.maxUniformBuffers + descriptor.maxTextures + descriptor.maxSamplers;
    size_t bufferSize = totalBindings * 32; // argument buffer must be 32 byte aligned, worst case of one descriptor per block

    Buffer::Descriptor buffDesc = { .size = bufferSize, .storageMode = ResourceStorageMode::hostVisible };
    m_argumentBuffer = std::dynamic_pointer_cast<MetalBuffer>(static_cast<std::shared_ptr<Buffer>>(m_device->newBuffer(buffDesc)));
}

std::unique_ptr<ParameterBlock> MetalParameterBlockPool::get(const std::shared_ptr<ParameterBlockLayout>& aPbLayout)
{
    std::scoped_lock lock(m_mutex);

    auto pbLayout = std::dynamic_pointer_cast<MetalParameterBlockLayout>(aPbLayout);

    std::unique_ptr<MetalParameterBlock> pBlock = std::make_unique<MetalParameterBlock>(m_argumentBuffer, m_nextOffset, pbLayout, this);
    size_t usedSize = sizeof(uint64_t) * pbLayout->bindings().size();
    m_nextOffset += (usedSize + 31uz) & ~31uz;
    m_usedParameterBlocks.insert(pBlock.get());
    return pBlock;
}

void MetalParameterBlockPool::release(ParameterBlock* aPBlock)
{
    std::scoped_lock lock(m_mutex);

    auto* pBlock = dynamic_cast<MetalParameterBlock*>(aPBlock);
    assert(m_usedParameterBlocks.contains(pBlock));
    m_usedParameterBlocks.erase(pBlock);
    if (m_usedParameterBlocks.empty())
        m_nextOffset = 0; // reset the pool
}

MetalParameterBlockPool::~MetalParameterBlockPool()
{
    for (auto* pBlock : m_usedParameterBlocks) {
        // blocks can outlive the pool, this prevent releasing a block to a freed pool
        pBlock->clearSourcePool();
    }
}

} // namespace gfx
