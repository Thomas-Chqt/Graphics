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

MetalBuffer::MetalBuffer(MetalBuffer&& other) noexcept
    : Buffer(ext::move(other)),
      m_usages(ext::exchange(other.m_usages, BufferUsage::uniformBuffer)),
      m_storageMode(ext::exchange(other.m_storageMode, ResourceStorageMode::hostVisible)),
      m_mtlBuffer(ext::exchange(other.m_mtlBuffer, nil))
{
}

MetalBuffer::MetalBuffer(const MetalDevice& device, const Buffer::Descriptor& desc)
    : m_usages(desc.usages),
      m_storageMode(desc.storageMode) { @autoreleasepool
{
    MTLResourceOptions ressourceOptions = desc.storageMode == ResourceStorageMode::deviceLocal ? MTLResourceStorageModePrivate : 0;

    m_mtlBuffer = [device.mtlDevice() newBufferWithLength:desc.size options:ressourceOptions];

    if (m_mtlBuffer == nil)
        throw ext::runtime_error("mtl buffer creation failed");
}}

size_t MetalBuffer::size() const { @autoreleasepool
{
    return m_mtlBuffer.length;
}}

void MetalBuffer::setContent(const void* data, size_t size) { @autoreleasepool
{
    void* content = m_mtlBuffer.contents;
    std::memcpy(content, data, size);
    if (m_mtlBuffer.storageMode == MTLStorageModeManaged)
        [m_mtlBuffer didModifyRange:NSMakeRange(0, size)];
}}

MetalBuffer::~MetalBuffer() { @autoreleasepool
{
    if (m_mtlBuffer != nil)
        [m_mtlBuffer release];
}}

void* MetalBuffer::contentVoid() { @autoreleasepool
{
    assert(mtlBuffer().storageMode == MTLStorageModeShared);
    return mtlBuffer().contents;
}}

MetalBuffer& MetalBuffer::operator = (MetalBuffer&& other) noexcept { @autoreleasepool
{
    Buffer::operator=(ext::move(other));
    if (this != &other)
    {
        if (m_mtlBuffer != nil)
            [m_mtlBuffer release];
        m_usages = ext::exchange(other.m_usages, BufferUsage::uniformBuffer);
        m_storageMode = ext::exchange(other.m_storageMode, ResourceStorageMode::hostVisible);
        m_mtlBuffer = ext::exchange(other.m_mtlBuffer, nil);
    }
    return *this;
}}

MetalBuffer::operator bool () const
{
    return m_mtlBuffer != nil;
}

}
