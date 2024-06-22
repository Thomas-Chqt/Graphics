/*
 * ---------------------------------------------------
 * flatColor.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 17:57:43
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "ShaderDatas.hpp"

using namespace metal;

struct VertexOut
{
    float3 pos;
    float4 clipPos [[position]];
    float2 uv;
    float3 normal;
};

vertex VertexOut simpleVertex(
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

fragment float4 flatColor_fs(
    VertexOut in [[stage_in]],

    constant float3& u_cameraPos [[buffer(1)]],

    constant float3& u_light_position          [[buffer(2)]],
    constant float3& u_light_color             [[buffer(3)]],
    constant float & u_light_ambiantIntensity  [[buffer(4)]],
    constant float & u_light_diffuseIntensity  [[buffer(5)]],
    constant float & u_light_specularIntensity [[buffer(6)]],

    constant float3& u_material_ambiant   [[buffer(7)]],
    constant float3& u_material_diffuse   [[buffer(8)]],
    constant float3& u_material_specular  [[buffer(9)]],
    constant float3& u_material_emissive  [[buffer(10)]],
    constant float & u_material_shininess [[buffer(11)]]
)
{
    float3 normal = normalize(in.normal);

    float3 ambiant = u_material_ambiant * (u_light_color * u_light_ambiantIntensity);
    float3 diffuse = float3(0.0);
    float3 specular = float3(0.0);

    float3 lightDirection = -normalize(u_light_position - in.pos);

    float diffuseFactor = dot(normal, -lightDirection);
    if (diffuseFactor > 0)
        diffuse = u_material_diffuse * (u_light_color * u_light_diffuseIntensity) * diffuseFactor;

    float specularFactor = dot(normalize(u_cameraPos - in.pos), normalize(reflect(lightDirection, normal)));
    if (specularFactor > 0)
        specular = u_material_specular * (u_light_color * u_light_specularIntensity) * pow(specularFactor, u_material_shininess);

    return float4(ambiant + diffuse + specular, 1.0);
}

fragment float4 lightCube_fs(
    VertexOut in [[stage_in]],

    constant float3& u_material_ambiant   [[buffer(1)]],
    constant float3& u_material_diffuse   [[buffer(2)]],
    constant float3& u_material_specular  [[buffer(3)]],
    constant float3& u_material_emissive  [[buffer(4)]],
    constant float & u_material_shininess [[buffer(5)]]
)
{
    return float4(u_material_emissive, 1.0);
}

fragment float4 textured_fs(
    VertexOut in [[stage_in]],

    constant float3& u_cameraPos [[buffer(1)]],

    constant float3& u_light_position          [[buffer(2)]],
    constant float3& u_light_color             [[buffer(3)]],
    constant float & u_light_ambiantIntensity  [[buffer(4)]],
    constant float & u_light_diffuseIntensity  [[buffer(5)]],
    constant float & u_light_specularIntensity [[buffer(6)]],

    constant float3& u_material_ambiant   [[buffer(7)]],
    constant float3& u_material_diffuse   [[buffer(8)]],
    constant float3& u_material_specular  [[buffer(9)]],
    constant float3& u_material_emissive  [[buffer(10)]],
    constant float & u_material_shininess [[buffer(11)]],

    texture2d<float> u_diffuseTexture [[texture(0)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);
    float4 diffuseTextel = u_diffuseTexture.sample(textureSampler, in.uv);

    float3 normal = normalize(in.normal);

    float3 ambiant = diffuseTextel.xyz * (u_light_color * u_light_ambiantIntensity);
    float3 diffuse = float3(0.0);
    float3 specular = float3(0.0);

    float3 lightDirection = -normalize(u_light_position - in.pos);

    float diffuseFactor = dot(normal, -lightDirection);
    if (diffuseFactor > 0)
        diffuse = diffuseTextel.xyz * (u_light_color * u_light_diffuseIntensity) * diffuseFactor;

    float specularFactor = dot(normalize(u_cameraPos - in.pos), normalize(reflect(lightDirection, normal)));
    if (specularFactor > 0)
        specular = u_material_specular * (u_light_color * u_light_specularIntensity) * pow(specularFactor, u_material_shininess);

    return float4(ambiant + diffuse + specular, diffuseTextel.w);
}