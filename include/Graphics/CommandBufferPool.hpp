/*
 * ---------------------------------------------------
 * CommandBufferPool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/07 10:04:47
 * ---------------------------------------------------
 */

#ifndef COMMANDBUFFERPOOL_HPP
#define COMMANDBUFFERPOOL_HPP

#include "Graphics/CommandBuffer.hpp"

#if defined(GFX_USE_UTILSCPP)
#else
    #include <memory>
#endif

namespace gfx
{

class CommandBufferPool
{
public:
    CommandBufferPool(const CommandBufferPool&) = delete;
    CommandBufferPool(CommandBufferPool&&) = delete;

    virtual ext::unique_ptr<CommandBuffer> get() = 0;
    virtual void release(ext::unique_ptr<CommandBuffer>&&) = 0;

    virtual ~CommandBufferPool() = default;

protected:
    CommandBufferPool() = default;

public:
    CommandBufferPool& operator=(const CommandBufferPool&) = delete;
    CommandBufferPool& operator=(CommandBufferPool&&) = delete;
};

} // namespace gfx

#endif // COMMANDBUFFERPOOL_HPP
