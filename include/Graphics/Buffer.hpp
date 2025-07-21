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
    namespace ext = utl;
#else
    #include <cstddef>
    //namespace ext = std;
#endif

namespace gfx
{

class Buffer
{
public:
    struct Descriptor
    {
        size_t size;
        void* data = nullptr;
        BufferUsages usage;
        ResourceStorageMode storageMode = ResourceStorageMode::hostVisible;
    };

public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&)      = delete;

    virtual void setContent(void* data, size_t size) = 0;

    virtual ~Buffer() = default;

protected:
    Buffer() = default;
    
public:
    Buffer& operator = (const Buffer&) = delete;
    Buffer& operator = (Buffer&&)      = delete;
};

}

#endif // BUFFER_HPP
