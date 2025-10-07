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

namespace gfx
{

MetalParameterBlockPool::MetalParameterBlockPool(const MetalDevice* device)
    : m_device(device)
{
    Buffer::Descriptor buffDesc = { .size = 10240, .storageMode = ResourceStorageMode::hostVisible };
    m_argumentBuffer = ext::dynamic_pointer_cast<MetalBuffer>((ext::shared_ptr<Buffer>)m_device->newBuffer(buffDesc));
}

ext::unique_ptr<ParameterBlock> MetalParameterBlockPool::get(const ParameterBlock::Layout& pbLayout)
{
    ext::unique_ptr<MetalParameterBlock> pBlock = ext::make_unique<MetalParameterBlock>(m_argumentBuffer, m_nextOffset, pbLayout, this);
    size_t usedSize = sizeof(uint64_t) * pbLayout.bindings.size();
    m_nextOffset += (usedSize + 31uz) & ~31uz;
    m_usedParameterBlocks.insert(pBlock.get());
    return pBlock;
}

void MetalParameterBlockPool::release(ParameterBlock* aPBlock)
{
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
