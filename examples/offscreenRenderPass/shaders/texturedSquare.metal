/*
 * ---------------------------------------------------
 * texturedSquare.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 14:40:01
 * ---------------------------------------------------
 */

#include <metal_stdlib>

struct VertexIn
{
    float2 pos [[attribute(0)]];
    float2 uv  [[attribute(1)]];
};

struct VertexOut
{
    float4 clipPos [[position]];
    float2 uv;
};

vertex VertexOut texturedSquare_vs(VertexIn in [[stage_in]])
{
    return {
        .clipPos = float4(in.pos.x, in.pos.y, 0.0, 1.0),
        .uv      = in.uv
    };
}

fragment float4 texturedSquare_fs(VertexOut in [[stage_in]],
    metal::texture2d<float> texture1 [[texture(0)]]
)
{
    constexpr metal::sampler default_sampler(metal::mag_filter::nearest, metal::min_filter::nearest);
    return texture1.sample(default_sampler, in.uv);
}