/*
 * ---------------------------------------------------
 * GraphicsPipeline.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/25 16:27:02
 * ---------------------------------------------------
 */

#ifndef GRAPHICSPIPELINE_HPP
#define GRAPHICSPIPELINE_HPP

#include "Graphics/ShaderFunction.hpp"
#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

class GraphicsPipeline
{
public:
    struct Descriptor
    {
        ShaderFunction* vertexShader;
        ShaderFunction* fragmentShader;

        ext::vector<PixelFormat> colorAttachmentPxFormats;
        ext::optional<PixelFormat> depthAttachmentPxFormat;
        
        BlendOperation blendOperation = BlendOperation::blendingOff;
    };

public:
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&&) = delete;

    virtual ~GraphicsPipeline() = default;

protected:
    GraphicsPipeline() = default;

public:
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
};

} // namespace gfx

#endif // GRAPHICSPIPELINE_HPP
