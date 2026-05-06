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

#include <cstdint>
#include <numeric>
#include <ranges>

namespace gfx
{

MetalParameterBlockPool::MetalParameterBlockPool(const MetalDevice* device, const ParameterBlockPool::Descriptor& descriptor)
    : m_device(device)
{
    size_t totalBindings = std::accumulate(descriptor.maxBindingCount.begin(), descriptor.maxBindingCount.end(), 0, [](auto acc, const auto& kv) { return acc + kv.second; });
    size_t bufferSize = totalBindings * 32; // argument buffer must be 32 byte aligned, worst case of one descriptor per block

    Buffer::Descriptor buffDesc = { .size = bufferSize, .storageMode = ResourceStorageMode::hostVisible };
    m_argumentBuffer = std::dynamic_pointer_cast<MetalBuffer>(static_cast<std::shared_ptr<Buffer>>(m_device->newBuffer(buffDesc)));
}

std::shared_ptr<ParameterBlock> MetalParameterBlockPool::get(const std::shared_ptr<ParameterBlockLayout>& aPbLayout)
{
    auto pbLayout = std::dynamic_pointer_cast<MetalParameterBlockLayout>(aPbLayout);
    assert(pbLayout);
    std::shared_ptr<MetalParameterBlock> pBlock;
    if (m_availablePBlocks.empty() == false) {
        pBlock = std::move(m_availablePBlocks.front());
        m_availablePBlocks.pop_front();
    }
    else {
        pBlock = std::make_shared<MetalParameterBlock>(pbLayout, m_argumentBuffer, m_nextOffset);
        const auto bindingCounts = pbLayout->bindings() | std::views::transform([](const auto& binding) { return binding.count; });
        size_t usedSize = sizeof(uint64_t) * std::accumulate(bindingCounts.begin(), bindingCounts.end(), 0u);
        m_nextOffset += (usedSize + 31uz) & ~31uz;
    }
    m_usedPBlocks.push_back(pBlock);
    return pBlock;
}

void MetalParameterBlockPool::reset()
{
    for (auto& pBlock : m_usedPBlocks) {
        pBlock->reuse();
        m_availablePBlocks.push_back(std::move(pBlock));
    }
    m_usedPBlocks.clear();
}

} // namespace gfx
