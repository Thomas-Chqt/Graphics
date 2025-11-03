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

#include "Graphics/ParameterBlock.hpp"
#include "Graphics/ShaderFunction.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/VertexLayout.hpp"

#include <vector>
#include <optional>

namespace gfx
{

class GraphicsPipeline
{
public:
    struct Descriptor
    {
        std::optional<VertexLayout> vertexLayout;

        ShaderFunction* vertexShader;
        ShaderFunction* fragmentShader;

        std::vector<PixelFormat> colorAttachmentPxFormats;
        std::optional<PixelFormat> depthAttachmentPxFormat;

        BlendOperation blendOperation = BlendOperation::blendingOff;
        CullMode cullMode = CullMode::none;

        std::vector<ParameterBlock::Layout> parameterBlockLayouts;
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
