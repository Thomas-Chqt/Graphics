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
#include "Metal/MetalDevice.hpp"

namespace gfx
{

MetalParameterBlockPool::MetalParameterBlockPool(const MetalDevice* device)
{
    Buffer::Descriptor buffDesc = {
        .size = 1024,
        .usage = BufferUsage::uniformBuffer, // dont matter (but not perFrameData)
        .storageMode = ResourceStorageMode::hostVisible};
    m_argumentBuffer = MetalBuffer(device, buffDesc);
}

MetalParameterBlock& MetalParameterBlockPool::get(const ParameterBlock::Layout& pbLayout)
{
    MetalParameterBlock& pbe = m_parameterBlocks.emplace_back(&m_argumentBuffer, m_nextOffset, pbLayout);
    size_t usedSize = sizeof(uint64_t) * pbLayout.bindings.size();
    m_nextOffset += (usedSize + 31uz) & ~31uz;
    return pbe;
}

void MetalParameterBlockPool::reset()
{
    m_parameterBlocks.clear();
    m_nextOffset = 0;
}

} // namespace gfx
