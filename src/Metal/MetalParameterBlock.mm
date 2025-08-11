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

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstddef>
    #include <utility>
    #include <cstdint>
    #include <memory>
    #include <cassert>
    namespace ext = std;
#endif

namespace gfx
{

MetalParameterBlock::MetalParameterBlock(MetalBuffer* argumentBuffer, size_t offset, const ParameterBlock::Layout& layout)
    : m_argumentBuffer(ext::move(argumentBuffer)), m_offset(offset), m_layout(layout)
{
    assert((argumentBuffer->size() - m_offset) >= (m_layout.bindings.size() * sizeof(uint64_t)));
}

void MetalParameterBlock::setBinding(uint32_t idx, const ext::shared_ptr<const Buffer>& aBuffer)
{
    auto buffer = std::dynamic_pointer_cast<const MetalBuffer>(aBuffer);
    assert(buffer);
    m_encodedBuffers.insert(ext::make_pair(m_layout.bindings[idx], buffer));
    ext::byte* content = m_argumentBuffer->content<ext::byte>() + m_offset;
    reinterpret_cast<uint64_t*>(content)[idx] = buffer->mtlBuffer().gpuAddress;
}

}
