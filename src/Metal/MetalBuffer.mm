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

namespace gfx
{

MetalBuffer::MetalBuffer(const MetalDevice* device, const Buffer::Descriptor& desc)
    : m_device(device),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode) { @autoreleasepool
{
    MTLResourceOptions ressourceOptions = desc.storageMode == ResourceStorageMode::deviceLocal ? MTLResourceStorageModePrivate : 0;

    if (desc.usages & BufferUsage::perFrameData)
        m_bufferCount = m_frameDatas.size();
    else
        m_bufferCount = 1;

    for (uint32_t i = 0; i < m_bufferCount; i++)
    {
        auto& frameData = m_frameDatas.at(i);

        if (desc.data == nullptr)
            frameData.mtlBuffer = [m_device->mtlDevice() newBufferWithLength:desc.size options:ressourceOptions];
        else
            frameData.mtlBuffer = [m_device->mtlDevice() newBufferWithBytes:desc.data length:desc.size options:ressourceOptions];

        if (frameData.mtlBuffer == nil)
            throw ext::runtime_error("mtl buffer creation failed");
    }
}}

size_t MetalBuffer::size() const { @autoreleasepool
{
    return mtlBuffer().length;
}}

void MetalBuffer::setContent(const void* data, size_t size) { @autoreleasepool
{
    for (uint32_t i = 0; i < m_bufferCount; i++)
    {
        auto& frameData = m_frameDatas.at(i);

        void* content = frameData.mtlBuffer.contents;
        std::memcpy(content, data, size);
        if (frameData.mtlBuffer.storageMode == MTLStorageModeManaged)
            [frameData.mtlBuffer didModifyRange:NSMakeRange(0, size)];
    }
}}

void MetalBuffer::clear() { @autoreleasepool
{
    for (uint32_t i = 0; i < m_bufferCount; i++)
    {
        auto& frameData = m_frameDatas.at(i);
        if (frameData.mtlBuffer != nil) {
            [frameData.mtlBuffer release];
            frameData.mtlBuffer = nil;
        }
    }
    m_bufferCount = 0;
    m_device = nullptr;
}}

MetalBuffer::~MetalBuffer()
{
    clear();
}

void* MetalBuffer::contentVoid() { @autoreleasepool
{
    assert(mtlBuffer().storageMode == MTLStorageModeShared);
    return mtlBuffer().contents;
}}

MetalBuffer::FrameData& MetalBuffer::currentFrameData()
{
    assert(m_device != nullptr);
    assert(m_bufferCount > 0);
    return m_frameDatas.at(m_device->currentFrameIdx() % m_bufferCount);
}

const MetalBuffer::FrameData& MetalBuffer::currentFrameData() const
{
    assert(m_device != nullptr);
    assert(m_bufferCount > 0);
    return m_frameDatas.at(m_device->currentFrameIdx() % m_bufferCount);
}

MetalBuffer& MetalBuffer::operator = (MetalBuffer&& other) noexcept
{
    if (this != &other)
    {
        clear();
        m_device = ext::exchange(other.m_device, nullptr);
        m_bufferCount = ext::exchange(other.m_bufferCount, 0);
        for (uint32_t i = 0; i < m_bufferCount; i++)
            m_frameDatas.at(i) = ext::exchange(other.m_frameDatas.at(i), FrameData());
    }
    return *this;
}

MetalBuffer::operator bool () const
{
    if (m_device == nullptr)
        return false;
    if (m_bufferCount == 0)
        return false;
    for (uint32_t i = 0; i < m_bufferCount; i++) {
        if (m_frameDatas.at(i).mtlBuffer == nil)
            return false;
    }
    return true;
}

}
