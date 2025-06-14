/*
 * ---------------------------------------------------
 * CommandBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 13:15:50
 * ---------------------------------------------------
 */

#ifndef COMMANDBUFFER_HPP
#define COMMANDBUFFER_HPP

#if defined(GFX_USE_UTILSCPP)
#else
#endif

namespace gfx
{

class CommandBuffer
{
public:
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&)      = delete;
    
    virtual ~CommandBuffer() = default;

protected:
    CommandBuffer() = default;
    
public:
    CommandBuffer& operator = (const CommandBuffer&) = delete;
    CommandBuffer& operator = (CommandBuffer&&)      = delete;
};

}

#endif // COMMANDBUFFER_HPP
