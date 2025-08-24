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
    : m_argumentBuffer(argumentBuffer), m_offset(offset), m_bindings(layout.bindings)
{
    assert((argumentBuffer->size() - m_offset) >= (m_bindings.size() * sizeof(uint64_t)));
}

void MetalParameterBlock::setBinding(uint32_t idx, const ext::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = ext::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);
    ext::byte* content = m_argumentBuffer->content<ext::byte>() + m_offset;
    reinterpret_cast<uint64_t*>(content)[idx] = buffer->mtlBuffer().gpuAddress;
    m_encodedBuffers.insert(ext::make_pair(m_bindings[idx], buffer));
}

}
