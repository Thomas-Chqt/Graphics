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
    : m_device(device) { @autoreleasepool
{
    MTLResourceOptions ressourceOptions = desc.storageMode == ResourceStorageMode::deviceLocal ? MTLResourceStorageModePrivate : 0;

    m_frameDatas.resize(desc.usage & BufferUsage::perFrameData ? m_device->maxFrameInFlight() : 1);
    for (auto& frameData : m_frameDatas)
    {
        if (desc.data == nullptr)
            frameData.mtlBuffer = [m_device->mtlDevice() newBufferWithLength:desc.size options:ressourceOptions];
        else
            frameData.mtlBuffer = [m_device->mtlDevice() newBufferWithBytes:desc.data length:desc.size options:ressourceOptions];

        if (frameData.mtlBuffer == Nil)
            throw ext::runtime_error("mtl buffer creation failed");
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
    }
}}

const id<MTLBuffer>& MetalBuffer::mtlBuffer() const { @autoreleasepool
{
    return m_frameDatas[m_device->currentFrameIdx() % m_frameDatas.size()].mtlBuffer;
}}

MetalBuffer::~MetalBuffer() { @autoreleasepool
{
    for (auto& frameData : m_frameDatas)
    {
        [frameData.mtlBuffer release];
    }
}}

void* MetalBuffer::contentVoid() { @autoreleasepool
{
    assert(mtlBuffer().storageMode == MTLStorageModeShared);
    return mtlBuffer().contents;
}}

}
