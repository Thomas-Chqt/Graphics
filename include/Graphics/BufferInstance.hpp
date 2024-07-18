/*
 * ---------------------------------------------------
 * BufferInstance.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 09:24:26
 * ---------------------------------------------------
 */

#ifndef BUFFERINSTANCE_HPP
# define BUFFERINSTANCE_HPP

#include "Buffer.hpp"
#include "GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

template<typename T>
class BufferInstance
{
public:
    BufferInstance()                      = delete;
    BufferInstance(const BufferInstance&) = delete;
    BufferInstance(BufferInstance&&)      = default;

    BufferInstance(const utils::SharedPtr<gfx::Buffer>& buffer) : m_buffer(buffer) 
    {
    }

    static inline BufferInstance alloc(const utils::SharedPtr<GraphicAPI>& api) { return BufferInstance(api->newBuffer(Buffer::Descriptor(sizeof(T)))); }

    inline void map() { m_content = (T*)m_buffer->mapContent(); }
    inline T& content() { return *m_content; }
    inline void unmap() { m_buffer->unMapContent(); }
    inline const utils::SharedPtr<gfx::Buffer>& buffer() { return m_buffer; }

private:
    T* m_content = nullptr;
    utils::SharedPtr<gfx::Buffer> m_buffer;

public:
    BufferInstance& operator = (const BufferInstance&) = delete;
    BufferInstance& operator = (BufferInstance&&)      = default;
};

}

#endif // BUFFERINSTANCE_HPP