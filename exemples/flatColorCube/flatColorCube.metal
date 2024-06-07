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

vertex float4 flatColorCube_vs(uint vID [[vertex_id]],
                               constant Vertex* vertices [[buffer(0)]],
                               constant metal::float4x4& u_MVPMatrix [[buffer(1)]])
{
    return u_MVPMatrix * float4(vertices[vID].x, vertices[vID].y, vertices[vID].z, 1.0);
}

fragment float4 flatColorCube_fs(constant float4& u_color [[buffer(0)]])
{
    return u_color;
}