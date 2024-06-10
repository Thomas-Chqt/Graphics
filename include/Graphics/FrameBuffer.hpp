/*
 * ---------------------------------------------------
 * FrameBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/10 13:51:44
 * ---------------------------------------------------
 */

#ifndef FRAMEBUFFER_HPP
# define FRAMEBUFFER_HPP

namespace gfx
{

class FrameBuffer
{
public:
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&&)      = delete;

    virtual ~FrameBuffer() = default;

protected:
    FrameBuffer() = default;

public:
    FrameBuffer& operator = (const FrameBuffer&) = delete;
    FrameBuffer& operator = (FrameBuffer&&)      = delete;
};

}

#endif // FRAMEBUFFER_HPP