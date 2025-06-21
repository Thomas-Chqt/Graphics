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

#include "Graphics/Framebuffer.hpp"
#include "Graphics/RenderPass.hpp"

#if defined(GFX_USE_UTILSCPP)
#else
#endif

namespace gfx
{

class CommandBuffer
{
public:
    CommandBuffer(const CommandBuffer&) = delete;

public:
    virtual void beginRenderPass(const ext::shared_ptr<RenderPass>&, const ext::shared_ptr<Framebuffer>&) = 0;
    virtual void endRenderPass(void) = 0;

    virtual ~CommandBuffer() = default;

protected:
    CommandBuffer() = default;
    CommandBuffer(CommandBuffer&&) = default;

public:
    CommandBuffer& operator=(const CommandBuffer&) = delete;

protected:
    CommandBuffer& operator=(CommandBuffer&&) = default;
};

} // namespace gfx

#endif // COMMANDBUFFER_HPP
