#ifndef PBR_MATERIAL_SLANG
#define PBR_MATERIAL_SLANG

#include "shader.h"

#ifndef __cplusplus

import SceneData;
import pbr;
import ibl;
import utils;

struct _VPMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_VPMatrix> vpMatrix;
#define vpMatrix vpMatrix.data

#ifdef __METAL__
[[vk::push_constant]] cbuffer ModelMatrix : register(b6)
#else
[[vk::push_constant]] cbuffer ModelMatrix
#endif
{
    float4x4 modelMatrix;
}

struct Vertex
{
    float3 pos;
    float2 uv;
    float3 normal;
    float4 tangent;
};

struct VSOutput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float2 uv;
    float3 tangent;
    float3 bitangent;
    float3 normal;
};

[shader("vertex")]
VSOutput vertexMain(Vertex input)
{
    const float4 worldPos = mul(float4(input.pos, 1.0), modelMatrix);
    const float3 bitangent = cross(input.normal, input.tangent.xyz) * input.tangent.w;
    VSOutput output;
    output.pos = worldPos.xyz;
    output.clipPos = mul(worldPos, vpMatrix);
    output.uv = input.uv;
    output.tangent = mul(input.tangent.xyz, (float3x3)modelMatrix);
    output.bitangent = mul(bitangent, (float3x3)modelMatrix);
    output.normal = mul(input.normal, (float3x3)modelMatrix);
    return output;
}

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

#else
namespace shader::pbr_material
{
#endif

struct MaterialData
{
    float4 baseColor;
    float3 emissiveFactor;
    FLOAT3_PADDING(0);
    float metallic;
    float roughness;
    int metallicTextureChannel;
    int roughnessTextureChannel;
};

#ifdef __cplusplus
}
#else

struct Material
{
    SamplerState sampler;
    Texture2D<float4> baseColorTexture;
    Texture2D<float3> normalTexture;
    Texture2D<float4> metallicTexture;
    Texture2D<float4> roughnessTexture;
    Texture2D<float4> emissiveTexture;

    SamplerState environmentSampler;
    TextureCube<float4> irradianceCubeMap;
    TextureCube<float4> prefilteredEnvironmentCubeMap;
    Texture2D<float2> brdfLut;

    ConstantBuffer<MaterialData> data;
};
ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    const float4 baseColor = material.data.baseColor * material.baseColorTexture.Sample(material.sampler, input.uv);
    const float3 emissive = material.data.emissiveFactor * material.emissiveTexture.Sample(material.sampler, input.uv).rgb;
    if (baseColor.a < 0.9 && emissive.r < 0.1 && emissive.g < 0.1 && emissive.b < 0.1 )
        discard;

    const float metallic = saturate(
        material.data.metallic *
        material.metallicTexture.Sample(material.sampler, input.uv)[material.data.metallicTextureChannel]
    );
    const float roughness = max(
        material.data.roughness *
        material.roughnessTexture.Sample(material.sampler, input.uv)[material.data.roughnessTextureChannel],
        0.01
    );

    const float3 normalMap = material.normalTexture.Sample(material.sampler, input.uv) * 2.0 - 1.0;
    const float3 T = normalize(input.tangent);
    const float3 B = normalize(input.bitangent);
    const float3 N = normalize(mul(normalMap, float3x3(T, B, normalize(input.normal))));
    const float3 V = normalize(sceneData.cameraPosition - input.pos);
    const float NdotV = max(dot(N, V), 0.0);

    pbr::FragmentContext fragCtx;
    fragCtx.position = input.pos;
    fragCtx.normal = N;
    fragCtx.cameraDir = V;
    fragCtx.baseColor = baseColor.rgb;
    fragCtx.metallic = metallic;
    fragCtx.roughness = roughness;

    float3 Lo = float3(0.0);
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        Lo += pbr::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        Lo += pbr::evalPointLight(fragCtx, sceneData.pointLights[i]);

    uint prefilteredWidth;
    uint prefilteredHeight;
    uint prefilteredMipCount;
    material.prefilteredEnvironmentCubeMap.GetDimensions(0, prefilteredWidth, prefilteredHeight, prefilteredMipCount);

    const float3 irradiance = material.irradianceCubeMap.Sample(material.environmentSampler, N).rgb;
    const float3 R = reflect(-V, N);
    const float mipLevel = roughness * float(prefilteredMipCount - 1);
    const float3 prefilteredColor = material.prefilteredEnvironmentCubeMap.SampleLevel(material.environmentSampler, R, mipLevel).rgb;
    const float2 environmentBrdf = material.brdfLut.SampleLevel(material.environmentSampler, float2(NdotV, roughness), 0.0).rg;
    const float3 F0 = lerp(float3(0.04), baseColor.rgb, metallic);

    const float3 ambient = shader::ibl::evaluateIBL(
        baseColor.rgb,
        metallic,
        roughness,
        NdotV,
        F0,
        irradiance,
        prefilteredColor,
        environmentBrdf
    );


    return float4(shader::acesToneMapping((Lo + ambient + emissive) * sceneData.exposure), baseColor.a);
}

#endif
#endif
