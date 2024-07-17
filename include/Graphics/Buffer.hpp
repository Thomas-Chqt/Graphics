/*
 * ---------------------------------------------------
 * Buffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 13:20:34
 * ---------------------------------------------------
 */

#ifndef BUFFER_HPP
# define BUFFER_HPP

#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class Buffer
{
public:
    struct Descriptor
    {
        Descriptor() = default;
        Descriptor(utils::uint64 size, const void* initialData = nullptr) : size(size), initialData(initialData) {};

        utils::String debugName;
        utils::uint64 size;
        const void* initialData = nullptr;
    };

public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&)      = delete;

    virtual utils::uint64 size() const = 0;

    virtual void* mapContent() = 0;
    virtual void unMapContent() = 0;
    
    virtual ~Buffer() = default;

protected:
    Buffer() = default;
    
public:
    Buffer& operator = (const Buffer&) = delete;
    Buffer& operator = (Buffer&&)      = delete;
};

template<typename T>
struct BufferInstance
{
    BufferInstance(const utils::SharedPtr<gfx::Buffer>& buffer) : buffer(buffer) {}

    inline void map() { content = buffer->mapContent(); }
    inline void unmap() { buffer->unMapContent(); }

    T* content;
    utils::SharedPtr<gfx::Buffer> buffer;
};

}

#endif // BUFFER_HPP