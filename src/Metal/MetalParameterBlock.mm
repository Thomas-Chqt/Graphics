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

#include <algorithm>
#include <cassert>
#include <numeric>
#include <ranges>

static_assert(sizeof(MTLResourceID) == sizeof(uint64_t), "MTLResourceID is not 64 bits");

namespace
{

uint32_t bindingOffset(const gfx::ParameterBlockLayout& layout, uint32_t idx)
{
    const auto bindingCounts = layout.bindings() | std::views::transform([](const auto& binding) { return binding.count; });
    return std::accumulate(bindingCounts.begin(), std::next(bindingCounts.begin(), idx), 0u);
}

uint32_t totalDescriptorCount(const gfx::ParameterBlockLayout& layout)
{
    const auto bindingCounts = layout.bindings() | std::views::transform([](const auto& binding) { return binding.count; });
    return std::accumulate(bindingCounts.begin(), bindingCounts.end(), 0u);
}

}

namespace gfx
{

MetalParameterBlock::MetalParameterBlock(const std::shared_ptr<MetalParameterBlockLayout>& layout, const std::shared_ptr<MetalBuffer>& argumentBuffer, size_t offset)
    : m_layout(layout),
      m_argumentBuffer(argumentBuffer),
      m_offset(offset)
{
    assert((argumentBuffer->size() - m_offset) >= (totalDescriptorCount(*m_layout) * sizeof(uint64_t)));
    m_encodedBuffers.resize(m_layout->bindings().size());
    m_encodedTextures.resize(m_layout->bindings().size());
    m_encodedSamplers.resize(m_layout->bindings().size());
}

void MetalParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Buffer>& aBuffer) { @autoreleasepool
{
    auto buffer = std::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);

    auto* content = std::bit_cast<uint64_t*>(m_argumentBuffer->content<std::byte>() + m_offset);
    content[bindingOffset(*m_layout, idx)] = buffer->mtlBuffer().gpuAddress;
    auto& encodedBuffers = m_encodedBuffers.at(idx);
    encodedBuffers.insert_or_assign(0, EncodedResource<MetalBuffer>{
        .resource = buffer,
        .binding = m_layout->bindings().at(idx)
    });
}}

void MetalParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Texture>& aTexture) { @autoreleasepool
{
    setBinding(idx, 0, aTexture);
}}

void MetalParameterBlock::setBinding(uint32_t idx, uint32_t arrayIndex, const std::shared_ptr<Texture>& aTexture) { @autoreleasepool
{
    setBinding(idx, arrayIndex, std::span(&aTexture, 1));
}}

void MetalParameterBlock::setBinding(uint32_t idx, uint32_t firstArrayIndex, std::span<const std::shared_ptr<Texture>> textures) { @autoreleasepool
{
    assert(m_layout->bindings().at(idx).type == BindingType::sampledTexture);
    assert(firstArrayIndex + textures.size() <= m_layout->bindings().at(idx).count);

    auto* content = std::bit_cast<MTLResourceID*>(m_argumentBuffer->content<std::byte>() + m_offset);
    const uint32_t offset = bindingOffset(*m_layout, idx);
    for (uint32_t i = firstArrayIndex; const auto& texturePtr : textures) {
        auto texture = std::dynamic_pointer_cast<MetalTexture>(texturePtr);
        assert(texture);
        content[offset + i] = texture->mtltexture().gpuResourceID;
        m_encodedTextures.at(idx).insert_or_assign(i, EncodedResource<MetalTexture>{
            .resource = texture,
            .binding = m_layout->bindings().at(idx)
        });
        ++i;
    }
}}

void MetalParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Sampler>& aSampler) { @autoreleasepool
{
    auto sampler = std::dynamic_pointer_cast<MetalSampler>(aSampler);
    assert(sampler);

    auto* content = std::bit_cast<MTLResourceID*>(m_argumentBuffer->content<std::byte>() + m_offset);
    content[bindingOffset(*m_layout, idx)] = sampler->mtlSamplerState().gpuResourceID;
    auto& encodedSamplers = m_encodedSamplers.at(idx);
    encodedSamplers.insert_or_assign(0, EncodedResource<MetalSampler>{
        .resource = sampler,
        .binding = m_layout->bindings().at(idx)
    });
}}

void MetalParameterBlock::clearBinding(uint32_t idx, uint32_t arrayIndex) { @autoreleasepool
{
    clearBinding(idx, arrayIndex, 1);
}}

void MetalParameterBlock::clearBinding(uint32_t idx, uint32_t firstArrayIndex, uint32_t count) { @autoreleasepool
{
    assert(count > 0);
    assert(firstArrayIndex + count <= m_layout->bindings().at(idx).count);

    auto eraseBindingRange = [firstArrayIndex, count]<typename T>(std::unordered_map<uint32_t, EncodedResource<T>>& resources) {
        std::erase_if(resources, [firstArrayIndex, count](const auto& entry) {
            return entry.first >= firstArrayIndex && entry.first < firstArrayIndex + count;
        });
    };

    switch (m_layout->bindings().at(idx).type)
    {
    case BindingType::constantBuffer:
    case BindingType::structuredBuffer:
        eraseBindingRange(m_encodedBuffers.at(idx));
        break;
    case BindingType::sampledTexture:
        eraseBindingRange(m_encodedTextures.at(idx));
        break;
    case BindingType::sampler:
        eraseBindingRange(m_encodedSamplers.at(idx));
        break;
    default:
        std::unreachable();
    }
}}

} // namespace gfx
