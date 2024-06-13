/*
 * ---------------------------------------------------
 * VertexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 14:24:33
 * ---------------------------------------------------
 */

#ifndef VERTEXBUFFER_HPP
# define VERTEXBUFFER_HPP

#include "Graphics/Enums.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class VertexBuffer
{
public:
    struct LayoutElement { int size; gfx::Type type; bool normalized; utils::uint32 stride; const void* pointer; };
    template<typename T> static utils::Array<LayoutElement> getLayout();

public:
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&)      = delete;

    virtual ~VertexBuffer() = default;

protected:
    VertexBuffer() = default;

public:
    VertexBuffer& operator = (const VertexBuffer&) = delete;
    VertexBuffer& operator = (VertexBuffer&&)      = delete;
};

}

#endif // VERTEXBUFFER_HPP