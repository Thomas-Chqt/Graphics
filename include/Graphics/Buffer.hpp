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

#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class Buffer
{
public:
    struct Descriptor
    {
        utils::String debugName;
        utils::uint64 size;
        const void* initialData = nullptr;
    };

public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&)      = delete;

    virtual utils::uint64 size() = 0;
    
    virtual ~Buffer() = default;

protected:
    Buffer() = default;
    
public:
    Buffer& operator = (const Buffer&) = delete;
    Buffer& operator = (Buffer&&)      = delete;
};

}

#endif // BUFFER_HPP