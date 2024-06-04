/*
 * ---------------------------------------------------
 * graphicPipeline.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:29:26
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 graphicPipeline_vertex(uint vID [[vertex_id]],
                                     constant float *vertices [[buffer(0)]])
{
    return float4((vertices + vID)[0], (vertices + vID)[1], 0.0, 1.0);
}

fragment float4 graphicPipeline_fragment()
{
    return float4(1.0, 1.0, 1.0, 1.0);
}
