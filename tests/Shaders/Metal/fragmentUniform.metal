/*
 * ---------------------------------------------------
 * fragmentUniform.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:11:53
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 fragmentUniform_vertex(uint vID [[vertex_id]],
                                     constant gfx_test::Vertex_fragmentUniform *vertices [[buffer(0)]])
{
    return float4(vertices[vID].x, vertices[vID].y, 0.0, 1.0);
}

fragment float4 fragmentUniform_fragment(constant float4* u_color [[buffer(0)]])
{
    return *u_color;
}