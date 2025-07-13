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
#include "Graphics/GraphicsPipeline.hpp"

#if defined(GFX_USE_UTILSCPP)
#else
    #include <memory>
    #include <cstdint>
#endif

namespace gfx
{

class CommandBuffer
{
public:
    CommandBuffer(const CommandBuffer&) = delete;

    virtual void beginRenderPass(const Framebuffer&) = 0;

    virtual void usePipeline(const ext::shared_ptr<const GraphicsPipeline>&) = 0;

    virtual void drawVertices(uint32_t start, uint32_t count) = 0;

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
