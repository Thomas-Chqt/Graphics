/*
 * ---------------------------------------------------
 * MetalParameterBlock.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 19:07:03
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalParameterBlockPool.hpp"

namespace gfx
{

MetalParameterBlock::MetalParameterBlock(const ext::shared_ptr<MetalBuffer>& argumentBuffer, size_t offset, const ParameterBlock::Layout& layout, MetalParameterBlockPool* srcPool)
    : m_argumentBuffer(argumentBuffer), m_offset(offset), m_layout(layout), m_sourcePool(srcPool)
{
    assert((argumentBuffer->size() - m_offset) >= (m_layout.bindings.size() * sizeof(uint64_t)));
}

void MetalParameterBlock::setBinding(uint32_t idx, const ext::shared_ptr<Buffer>& aBuffer) { @autoreleasepool
{
    auto buffer = ext::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);

    auto* content = ext::bit_cast<uint64_t*>(m_argumentBuffer->content<ext::byte>() + m_offset);
    content[idx] = buffer->mtlBuffer().gpuAddress;

    m_encodedBuffers.insert(ext::make_pair(buffer, m_layout.bindings[idx]));
}}

MetalParameterBlock::~MetalParameterBlock()
{
    if (m_sourcePool != nullptr)
        m_sourcePool->release(this);
}

} // namespace gfx
