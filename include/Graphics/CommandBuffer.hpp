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
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"
#include "Graphics/Drawable.hpp"

#if defined(GFX_USE_UTILSCPP)
#else
    #include <memory>
    #include <cstdint>
#endif

#if defined(GFX_IMGUI_ENABLED)
    struct ImDrawData;
#endif

namespace gfx
{

class CommandBufferPool;

class CommandBuffer
{
public:
    CommandBuffer(const CommandBuffer&) = delete;

    virtual CommandBufferPool* pool() = 0;

    virtual void beginRenderPass(const Framebuffer&) = 0;

    virtual void usePipeline(const ext::shared_ptr<const GraphicsPipeline>&) = 0;
    virtual void useVertexBuffer(const ext::shared_ptr<Buffer>&) = 0;

    virtual void setParameterBlock(const ext::shared_ptr<const ParameterBlock>&, uint32_t index) = 0;

    virtual void drawVertices(uint32_t start, uint32_t count) = 0;
    virtual void drawIndexedVertices(const ext::shared_ptr<Buffer>& idxBuffer) = 0;

#if defined(GFX_IMGUI_ENABLED)
    virtual void imGuiRenderDrawData(ImDrawData*) const = 0;
#endif

    virtual void endRenderPass() = 0;


    virtual void beginBlitPass() = 0;

    virtual void copyBufferToBuffer(const ext::shared_ptr<Buffer>& src, const ext::shared_ptr<Buffer>& dst, size_t size) = 0;
    virtual void copyBufferToTexture(const ext::shared_ptr<Buffer>&, const ext::shared_ptr<Texture>&) = 0;

    virtual void endBlitPass() = 0;

    virtual void presentDrawable(const ext::shared_ptr<Drawable>&) = 0;

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
