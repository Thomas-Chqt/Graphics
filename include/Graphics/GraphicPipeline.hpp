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

namespace gfx
{

class GraphicPipeline
{
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