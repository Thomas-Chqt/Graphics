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

#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "Enums.hpp"

namespace gfx
{

class GraphicPipeline
{
public:
    struct Descriptor
    {
        utils::String metalVSFunction;
        utils::String metalFSFunction;

        utils::String openglVSCode;
        utils::String openglFSCode;

        gfx::ColorPixelFormat colorPixelFormat = ColorPixelFormat::BGRA;
        gfx::DepthPixelFormat depthPixelFormat = DepthPixelFormat::Depth32;
        
        gfx::BlendOperation blendOperation = BlendOperation::srcA_plus_1_minus_srcA;
    };

public:
    GraphicPipeline(const GraphicPipeline&) = delete;
    GraphicPipeline(GraphicPipeline&&)      = delete;

    virtual utils::uint32 findVertexUniformIndex(const utils::String& name) = 0;
    virtual utils::uint32 findFragmentUniformIndex(const utils::String& name) = 0;

    virtual ~GraphicPipeline() = default;

protected:
    GraphicPipeline() = default;

public:
    GraphicPipeline& operator = (const GraphicPipeline&) = delete;
    GraphicPipeline& operator = (GraphicPipeline&&)      = delete;
};

} // namespace gfx

#endif // GRAPHICSPIPELINE_HPP
