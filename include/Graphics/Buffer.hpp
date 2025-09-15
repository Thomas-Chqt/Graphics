/*
 * ---------------------------------------------------
 * Buffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 06:45:08
 * ---------------------------------------------------
 */

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl; // NOLINT
#else
    #include <cstddef>
    namespace ext = std; // NOLINT
#endif

namespace gfx
{

class Buffer
{
public:
    struct Descriptor
    {
        size_t size = 0;
        BufferUsages usages = BufferUsage::uniformBuffer;
        ResourceStorageMode storageMode = ResourceStorageMode::hostVisible;
    };

public:
    Buffer(const Buffer&) = delete;

    virtual size_t size() const = 0;
    virtual BufferUsages usages() const = 0;
    virtual ResourceStorageMode storageMode() const = 0;

    template<typename T> inline T* content() { return (T*)contentVoid(); }
    virtual void setContent(const void* data, size_t size) = 0;
    inline void setContent(const auto& data) { setContent(&data, sizeof(data)); }

    virtual ~Buffer() = default;

protected:
    Buffer() = default;
    Buffer(Buffer&&) = default;

    virtual void* contentVoid() = 0;
    
public:
    Buffer& operator = (const Buffer&) = delete;

protected:
    Buffer& operator = (Buffer&&) = default;
};

}

#endif // BUFFER_HPP
