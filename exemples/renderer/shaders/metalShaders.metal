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

float3 computePointLight(constant PointLight& light, Material material, float3 fragPos, float3 normal, float3 cameraDir)
{
    float3 lightDir = normalize(light.position - fragPos);

    float diffuseFactor = dot(normal, lightDir);
    float specularFactor = dot(reflect(-lightDir, normal), cameraDir);

    float3 ambiant  = material.ambiant  * light.color * light.ambiantIntensity;
    float3 diffuse  = material.diffuse  * light.color * light.diffuseIntensity * max(diffuseFactor, 0.0f);
    float3 specular = material.specular * light.color * light.specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, material.shininess) : 0.0f);
    
    float3 emissive = material.emissive;

    return ambiant + diffuse + specular + emissive;
}

float4 computePixelColor(VertexOut in,
    constant float3& u_cameraPos,
    constant PointLight& u_light,
    Material u_material         
)
{
    float3 normal = normalize(in.normal);
    float3 cameraDir = normalize(u_cameraPos - in.pos);

    float3 fragmentColor = computePointLight(u_light, u_material, in.pos, normal, cameraDir);

    return float4(fragmentColor, 1.0);
}

fragment float4 flatColor_fs(VertexOut in [[stage_in]],
    constant float3& u_cameraPos  [[buffer(1)]],
    constant PointLight& u_light  [[buffer(2)]],
    constant Material& u_material [[buffer(3)]]
)
{
    return computePixelColor(in, u_cameraPos, u_light, u_material);
}

fragment float4 textured_fs(VertexOut in [[stage_in]],
    constant float3& u_cameraPos      [[buffer(1)]],
    constant PointLight& u_light      [[buffer(2)]],
    constant Material& u_material     [[buffer(3)]],
    texture2d<float> u_diffuseTexture [[texture(0)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);
    float4 diffuseTextel = u_diffuseTexture.sample(textureSampler, in.uv);

    Material newMaterial = u_material;
    newMaterial.diffuse = diffuseTextel.xyz;
    newMaterial.ambiant = diffuseTextel.xyz;

    return computePixelColor(in, u_cameraPos, u_light, newMaterial);
}