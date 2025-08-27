/*
 * ---------------------------------------------------
 * MetalBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 08:11:10
 * ---------------------------------------------------
 */

#ifndef METALBUFFER_HPP
#define METALBUFFER_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"

namespace gfx
{

class MetalDevice;

class MetalBuffer : public Buffer
{
public:
    MetalBuffer() = default;
    MetalBuffer(const MetalBuffer&) = delete;
    MetalBuffer(MetalBuffer&&) = default;

    MetalBuffer(const MetalDevice*, const Buffer::Descriptor&);

    size_t size() const override;
    inline BufferUsages usages() const override { return m_usages; };
    inline ResourceStorageMode storageMode() const override { return m_storageMode; };

    void setContent(const void* data, size_t size) override;

    const id<MTLBuffer>& mtlBuffer() const { return currentFrameData().mtlBuffer; }

    void clear();

    ~MetalBuffer() override;

protected:
    void* contentVoid() override;

private:
    struct FrameData
    {
        id<MTLBuffer> mtlBuffer = nil;
    };

    FrameData& currentFrameData();
    const FrameData& currentFrameData() const;

    const MetalDevice* m_device = nullptr;
    BufferUsages m_usages = BufferUsage::uniformBuffer;
    ResourceStorageMode m_storageMode = ResourceStorageMode::hostVisible;

    uint32_t m_bufferCount = 0;
    PerFrameInFlight<FrameData> m_frameDatas;

public:
    MetalBuffer& operator = (const MetalBuffer&) = delete;
    MetalBuffer& operator = (MetalBuffer&&) noexcept;
    operator bool () const;
};

}

#endif // METALBUFFER_HPP
