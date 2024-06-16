/*
 * ---------------------------------------------------
 * texturedCube.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:20:14
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

struct VertexOut
{
    float4 pos [[position]];
    float2 uv;
};

vertex VertexOut texturedCube_vs(uint vID [[vertex_id]],
                                   constant Vertex *vertices [[buffer(0)]],
                                   constant metal::float4x4& u_MVPMatrix [[buffer(1)]])
{
    return (VertexOut){
        .pos = u_MVPMatrix * float4(vertices[vID].pos, 1.0),
        .uv  = vertices[vID].uv
    };
}

fragment float4 texturedCube_fs(VertexOut in [[stage_in]], metal::texture2d<float> u_texture [[texture(0)]])
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    return u_texture.sample(textureSampler, in.uv);
}