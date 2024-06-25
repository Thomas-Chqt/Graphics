/*
 * ---------------------------------------------------
 * baseTexture.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/25 13:01:13
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "lightCompute.metal.h"

using namespace metal;

struct VertexOut
{
    float3 pos;
    float4 clipPos [[position]];
    float2 uv;
    float3 normal;
};

struct Light
{
    float3 position;
    float3 color;
    float  ambiantIntensity;
    float  diffuseIntensity;
    float  specularIntensity;
};

struct Material
{
    float3 specular;
    float3 emissive;
    float  shininess;
};

fragment float4 baseTexture(VertexOut in [[stage_in]],
    constant float3& u_cameraPos      [[buffer(1)]],
    constant Light* u_lights          [[buffer(2)]],
    constant Material& u_material     [[buffer(3)]],
    texture2d<float> u_diffuseTexture [[texture(0)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    float4 baseColor = u_diffuseTexture.sample(textureSampler, in.uv);

    FragmentInfos fragInfos = {
        .position  = in.pos,
        .normal    = normalize(in.normal),
        .ambiant   = baseColor,
        .diffuse   = baseColor,
        .specular  = u_material.specular,
        .emissive  = u_material.emissive,
        .shininess = u_material.shininess
    };

    float3 cameraDir = normalize(u_cameraPos - in.pos);

    float3 output = computePointLight(u_lights[0], fragInfos, cameraDir);

    return float4(output, baseColor.w);

}