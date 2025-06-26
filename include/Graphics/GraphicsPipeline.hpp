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

#include "Graphics/ShaderLib.hpp"
#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <string>
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

struct ShaderStage
{
    ShaderLib* shaderLib;
    ext::string functionName;
};

class GraphicsPipeline
{
public:
    struct Descriptor
    {
        ext::vector<ShaderStage> shaderStages;

        ext::vector<PixelFormat> colorAttachmentPxFormats;
        ext::optional<PixelFormat> depthAttachmentPxFormat;
        
        BlendOperation blendOperation = BlendOperation::blendingOff;
    };

public:
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&&) = delete;

    virtual ~GraphicsPipeline() = default;

private:
    GraphicsPipeline() = default;

public:
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
};

} // namespace gfx

#endif // GRAPHICSPIPELINE_HPP
