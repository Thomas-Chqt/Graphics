/*
 * ---------------------------------------------------
 * IndexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 12:21:45
 * ---------------------------------------------------
 */

#ifndef INDEXBUFFER_HPP
# define INDEXBUFFER_HPP

#include "UtilsCPP/Types.hpp"

namespace gfx
{

class IndexBuffer
{
public:
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&)      = delete;

    virtual utils::uint64 indexCount() = 0;

    virtual ~IndexBuffer() = default;

protected:
    IndexBuffer() = default;

public:
    IndexBuffer& operator = (const IndexBuffer&) = delete;
    IndexBuffer& operator = (IndexBuffer&&)      = delete;      
};

}

#endif // INDEXBUFFER_HPP