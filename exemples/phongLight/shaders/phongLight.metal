/*
 * ---------------------------------------------------
 * phongLight.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/19 18:22:10
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

using namespace metal;

struct VertexOut
{
    float4 pos [[position]];
    float2 uv;
    float3 normal;
};

vertex VertexOut phongLight_vs(
    uint vID                         [[vertex_id]],
    constant Vertex* vertices        [[buffer(0)]],
    constant float4x4& u_modelMatrix [[buffer(1)]],
    constant float4x4& u_vpMatrix    [[buffer(2)]]
)
{
    return (VertexOut){
        .pos    = u_vpMatrix * u_modelMatrix * float4(vertices[vID].pos, 1.0),
        .uv     = vertices[vID].uv,
        .normal = (u_modelMatrix * float4(vertices[vID].normal, 0)).xyz
    };
}

fragment float4 phongLight_fs(
    VertexOut in                         [[stage_in ]],
    constant float3& u_lightColor        [[buffer(1)]],
    constant float3& u_lightDirection    [[buffer(2)]],
    constant float & u_ambiantIntensity  [[buffer(3)]],
    constant float & u_diffuseIntensity  [[buffer(4)]],
    constant float & u_specularIntensity [[buffer(5)]],
    constant float & u_specularPower     [[buffer(6)]],
    constant float3& u_cameraPos         [[buffer(7)]]
)
{
    float3 baseColor     = float3(1.0, 1.0, 1.0);
    float3 ambiantColor  = u_lightColor * u_ambiantIntensity;
    float3 diffuseColor  = float3(0, 0, 0);
    float3 specularColor = float3(0, 0, 0);

    float diffuseFactor = dot(normalize(in.normal), -u_lightDirection);
    if (diffuseFactor > 0)
        diffuseColor = u_lightColor * u_diffuseIntensity * diffuseFactor;

    float specularFactor = dot(normalize(u_cameraPos - in.pos.xyz), normalize(reflect(u_lightDirection, normalize(in.normal))));
    if (specularFactor > 0)
        specularColor = u_lightColor * u_specularIntensity * pow(specularFactor, u_specularPower);

    return float4(baseColor * (ambiantColor + diffuseColor + specularColor), 1);
}
