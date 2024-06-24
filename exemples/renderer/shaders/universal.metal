/*
 * ---------------------------------------------------
 * universal.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 17:57:43
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "ShaderDatas.hpp"
#include "lightCompute.metal.h"

using namespace metal;

struct VertexOut
{
    float3 pos;
    float4 clipPos [[position]];
    float2 uv;
    float3 normal;
};

vertex VertexOut universal_vs(
    uint vID                         [[vertex_id]],
    constant Vertex* vertices        [[buffer(0)]],
    constant float4x4& u_modelMatrix [[buffer(1)]],
    constant float4x4& u_vpMatrix    [[buffer(2)]]
)
{
    return (VertexOut){
        .pos     = (u_modelMatrix * float4(vertices[vID].pos, 1.0)).xyz,
        .clipPos = u_vpMatrix * u_modelMatrix * float4(vertices[vID].pos, 1.0),
        .uv      = vertices[vID].uv,
        .normal  = (u_modelMatrix * float4(vertices[vID].normal, 0)).xyz
    };
}

fragment float4 universal_fs(VertexOut in [[stage_in]],
    constant float3& u_cameraPos       [[buffer(1)]],
    constant PointLight* u_pointLights [[buffer(2)]],
    constant Material& u_material      [[buffer(3)]],
    texture2d<float> u_diffuseTexture  [[texture(0)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    float4 ambiantColor = float4(u_material.ambiant, 1.0f);
    float4 diffuseColor = float4(u_material.diffuse, 1.0f);

    if(u_material.useDiffuseMap)
    {
        ambiantColor = u_diffuseTexture.sample(textureSampler, in.uv);
        diffuseColor = ambiantColor;
    }

    FragmentInfos fragInfos = {
        .position  = in.pos,
        .normal    = normalize(in.normal),
        .ambiant   = ambiantColor.xyz,
        .diffuse   = diffuseColor.xyz,
        .specular  = u_material.specular,
        .emissive  = u_material.emissive,
        .shininess = u_material.shininess
    };


    float3 cameraDir = normalize(u_cameraPos - in.pos);

    float3 output = computePointLight(u_pointLights[0], fragInfos, cameraDir);

    if(u_material.useDiffuseMap)
        return float4(output, diffuseColor.w);
    else
        return float4(output, diffuseColor.w);

}
