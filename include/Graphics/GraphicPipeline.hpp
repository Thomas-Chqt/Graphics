/*
 * ---------------------------------------------------
 * GraphicPipeline.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:27:37
 * ---------------------------------------------------
 */

#ifndef GRAPHICSPIPELINE_HPP
# define GRAPHICSPIPELINE_HPP

#include "Shader.hpp"
#include "VertexLayout.hpp"

#include "Enums.hpp"

namespace gfx
{

class GraphicPipeline
{
public:
    struct Descriptor
    {
        VertexLayout vertexLayout;

        Shader* vertexShader = nullptr;
        Shader* fragmentShader = nullptr;

        PixelFormat colorPixelFormat = PixelFormat::BGRA;
        PixelFormat depthPixelFormat = PixelFormat::Depth32;
        
        BlendOperation blendOperation = BlendOperation::srcA_plus_1_minus_srcA;
    };

public:
    GraphicPipeline(const GraphicPipeline&) = delete;
    GraphicPipeline(GraphicPipeline&&)      = delete;

    virtual ~GraphicPipeline() = default;

protected:
    GraphicPipeline() = default;

public:
    GraphicPipeline& operator = (const GraphicPipeline&) = delete;
    GraphicPipeline& operator = (GraphicPipeline&&)      = delete;
};

} // namespace gfx

#endif // GRAPHICSPIPELINE_HPP
