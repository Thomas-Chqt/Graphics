/*
 * ---------------------------------------------------
 * MetalBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 09:07:05
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/Buffer.hpp"

#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalDevice.hpp"

#import <Metal/Metal.h>
#include <cstdint>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    #include <cstring>
    #include <utility>
    #include <cassert>
    #include <cstddef>
    namespace ext = std;
#endif

namespace gfx
{

MetalBuffer::MetalBuffer(const MetalDevice* device, const Buffer::Descriptor& desc)
    : m_device(device), m_usages(desc.usages) { @autoreleasepool
{
    MTLResourceOptions ressourceOptions = desc.storageMode == ResourceStorageMode::deviceLocal ? MTLResourceStorageModePrivate : 0;

    for (auto& frameData :  m_frameDatas)
    {
        if (desc.data == nullptr)
            frameData.mtlBuffer = [m_device->mtlDevice() newBufferWithLength:desc.size options:ressourceOptions];
        else
            frameData.mtlBuffer = [m_device->mtlDevice() newBufferWithBytes:desc.data length:desc.size options:ressourceOptions];

        if (frameData.mtlBuffer == Nil)
            throw ext::runtime_error("mtl buffer creation failed");

        if (static_cast<bool>(desc.usages & BufferUsage::perFrameData) == false)
            break;
    }
}}

size_t MetalBuffer::size() const { @autoreleasepool
{
    return mtlBuffer().length;
}}

void MetalBuffer::setContent(const void* data, size_t size) { @autoreleasepool
{
    for (auto& frameData : m_frameDatas)
    {
        void* content = frameData.mtlBuffer.contents;
        std::memcpy(content, data, size);
        if (frameData.mtlBuffer.storageMode == MTLStorageModeManaged)
            [frameData.mtlBuffer didModifyRange:NSMakeRange(0, size)];

        if (static_cast<bool>(m_usages & BufferUsage::perFrameData) == false)
            break;
    }
}}

const id<MTLBuffer>& MetalBuffer::mtlBuffer() const { @autoreleasepool
{
    return currentFrameData().mtlBuffer;
}}

void* MetalBuffer::contentVoid() { @autoreleasepool
{
    assert(mtlBuffer().storageMode == MTLStorageModeShared);
    return mtlBuffer().contents;
}}

MetalBuffer::FrameData::FrameData(FrameData&& other) noexcept
    : mtlBuffer(other.mtlBuffer)
{
    other.mtlBuffer = nil;
}

MetalBuffer::FrameData::~FrameData() { @autoreleasepool
{
    if (mtlBuffer != nil)
        [mtlBuffer release];
}}

MetalBuffer::FrameData& MetalBuffer::FrameData::operator = (FrameData&& other) noexcept
{
    if (this != &other)
    {
        mtlBuffer = other.mtlBuffer;
        other.mtlBuffer = nil;
    }
    return *this;
}

MetalBuffer::FrameData& MetalBuffer::currentFrameData()
{
    uint64_t bufferCount = static_cast<bool>(m_usages & BufferUsage::perFrameData) ? m_frameDatas.size() : 1;
    return m_frameDatas.at(m_device->currentFrameIdx() % bufferCount);
}

const MetalBuffer::FrameData& MetalBuffer::currentFrameData() const
{
    uint64_t bufferCount = static_cast<bool>(m_usages & BufferUsage::perFrameData) ? m_frameDatas.size() : 1;
    return m_frameDatas.at(m_device->currentFrameIdx() % bufferCount);
}

}
