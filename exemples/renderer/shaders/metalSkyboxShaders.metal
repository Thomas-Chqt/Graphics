/*
 * ---------------------------------------------------
 * metalSkyboxShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/16 20:28:54
 * ---------------------------------------------------
 */

#include <metal_stdlib>

using namespace metal;

struct VertexIn
{
    float3 pos [[attribute(0)]];
};

struct VertexOut
{
    float4   clipPos [[position]];
    float3   uv;
};

vertex VertexOut skybox_vs(VertexIn in [[stage_in]], constant float4x4& vpMatrix [[buffer(1)]])
{
    return {
        .clipPos = (vpMatrix * float4(in.pos, 1.0)).xyww,
        .uv = in.pos
    };
}

fragment float4 skybox_fs(VertexOut in [[stage_in]], texturecube<float> skyboxTexture [[texture(0)]])
{
    constexpr metal::sampler skyboxTextureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    return skyboxTexture.sample(skyboxTextureSampler, in.uv);
}