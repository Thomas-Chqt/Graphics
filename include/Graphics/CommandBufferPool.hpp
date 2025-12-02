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

#include <memory>

namespace gfx
{

class CommandBufferPool
{
public:
    CommandBufferPool(const CommandBufferPool&) = delete;
    CommandBufferPool(CommandBufferPool&&) = delete;

    // do not call get() and reset() concurently.
    // should have one pool per thead so no problem

    virtual std::shared_ptr<CommandBuffer> get() = 0;
    virtual void reset() = 0;

    virtual ~CommandBufferPool() = default;

protected:
    CommandBufferPool() = default;

public:
    CommandBufferPool& operator=(const CommandBufferPool&) = delete;
    CommandBufferPool& operator=(CommandBufferPool&&) = delete;
};

} // namespace gfx

#endif // COMMANDBUFFERPOOL_HPP
