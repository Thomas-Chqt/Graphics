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

static_assert(sizeof(MTLResourceID) == sizeof(uint64_t), "MTLResourceID is not 64 bits");

namespace gfx
{

MetalParameterBlock::MetalParameterBlock(const std::shared_ptr<MetalBuffer>& argumentBuffer, size_t offset, const std::shared_ptr<MetalParameterBlockLayout>& layout, MetalParameterBlockPool* srcPool)
    : m_argumentBuffer(argumentBuffer), m_offset(offset), m_layout(layout), m_sourcePool(srcPool)
{
    assert((argumentBuffer->size() - m_offset) >= (m_layout->bindings().size() * sizeof(uint64_t)));
}

void MetalParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Buffer>& aBuffer) { @autoreleasepool
{
    auto buffer = std::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);

    auto* content = std::bit_cast<uint64_t*>(m_argumentBuffer->content<std::byte>() + m_offset);
    content[idx] = buffer->mtlBuffer().gpuAddress;

    m_encodedBuffers.insert(std::make_pair(buffer, m_layout->bindings()[idx]));
}}

void MetalParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Texture>& aTexture) { @autoreleasepool
{
    auto texture = std::dynamic_pointer_cast<MetalTexture>(aTexture);
    assert(texture);

    auto* content = std::bit_cast<MTLResourceID*>(m_argumentBuffer->content<std::byte>() + m_offset);
    content[idx] = texture->mtltexture().gpuResourceID;

    m_encodedTextures.insert(std::make_pair(texture, m_layout->bindings()[idx]));
}}

void MetalParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Sampler>& aSampler) { @autoreleasepool
{
    auto sampler = std::dynamic_pointer_cast<MetalSampler>(aSampler);
    assert(sampler);

    auto* content = std::bit_cast<MTLResourceID*>(m_argumentBuffer->content<std::byte>() + m_offset);
    content[idx] = sampler->mtlSamplerState().gpuResourceID;

    m_encodedSamplers.insert(std::make_pair(sampler, m_layout->bindings()[idx]));
}}

MetalParameterBlock::~MetalParameterBlock()
{
    if (m_sourcePool != nullptr)
        m_sourcePool->release(this);
}

} // namespace gfx
