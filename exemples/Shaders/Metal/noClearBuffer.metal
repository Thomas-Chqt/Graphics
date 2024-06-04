/*
 * ---------------------------------------------------
 * noClearBuffer.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/30 10:53:18
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 noClearBuffer_vertex(uint                                     vID         [[vertex_id]],
                                   constant gfx_test::Vertex_noClearBuffer* vertices    [[buffer(0)]],
                                   constant metal::float3x3&                u_MVPMatrix [[buffer(1)]])
{
    float3 newPos = u_MVPMatrix * float3(vertices[vID].pos, 1.0);
    return float4(newPos, 1.0);
}

fragment float4 noClearBuffer_fragment(constant float4& u_color [[buffer(0)]])
{
    return u_color;
}