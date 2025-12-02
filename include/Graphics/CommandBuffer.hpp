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

#include <memory>
#include <cstdint>

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

    virtual void beginRenderPass(const Framebuffer&) = 0;

    virtual void usePipeline(const std::shared_ptr<const GraphicsPipeline>&) = 0;
    virtual void useVertexBuffer(const std::shared_ptr<Buffer>&) = 0;

    virtual void setParameterBlock(const std::shared_ptr<const ParameterBlock>&, uint32_t index) = 0;
    virtual void setPushConstants(const void* data, size_t size) = 0;
    void setPushConstants(const auto* data) { setPushConstants(data, sizeof(decltype(*data))); }

    virtual void drawVertices(uint32_t start, uint32_t count) = 0;
    virtual void drawIndexedVertices(const std::shared_ptr<Buffer>& idxBuffer) = 0;

#if defined(GFX_IMGUI_ENABLED)
    virtual void imGuiRenderDrawData(ImDrawData*) const = 0;
#endif

    virtual void endRenderPass() = 0;


    virtual void beginBlitPass() = 0;

    virtual void copyBufferToBuffer(const std::shared_ptr<Buffer>& src, const std::shared_ptr<Buffer>& dst, size_t size) = 0;
    virtual void copyBufferToTexture(const std::shared_ptr<Buffer>& buffer, size_t bufferOffset, const std::shared_ptr<Texture>& texture, uint32_t layerIndex = 0) = 0;
    inline void copyBufferToTexture(const std::shared_ptr<Buffer>& buffer, const std::shared_ptr<Texture>& texture) { copyBufferToTexture(buffer, 0, texture); }

    virtual void endBlitPass() = 0;

    virtual void presentDrawable(const std::shared_ptr<Drawable>&) = 0;

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
