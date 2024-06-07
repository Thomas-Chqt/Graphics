/*
 * ---------------------------------------------------
 * fragmentUniform.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:25:42
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 fragmentUniform_vs(uint vID [[vertex_id]],
                                     constant Vertex* vertices [[buffer(0)]])
{
    return float4(vertices[vID].x, vertices[vID].y, 0.0, 1.0);
}

fragment float4 fragmentUniform_fs(constant float4& u_color [[buffer(0)]])
{
    return u_color;
}