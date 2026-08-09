/*
 * ---------------------------------------------------
 * textured.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/30 08:54:54
 * ---------------------------------------------------
 */

#ifndef TEXTURED_SLANG
#define TEXTURED_SLANG

#include "shader.h"

#ifndef __cplusplus

import SceneData;
import phong_shading;

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
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix);
    float3 bitangent = cross(input.normal, input.tangent.xyz) * input.tangent.w;
    VSOutput output;
    output.pos       = worldPos.xyz;
    output.clipPos   = mul(worldPos, vpMatrix);
    output.uv        = input.uv;
    output.tangent   = mul(input.tangent.xyz, (float3x3)modelMatrix);
    output.bitangent = mul(bitangent, (float3x3)modelMatrix);
    output.normal    = mul(input.normal, (float3x3)modelMatrix);
    return output;
}

/* -------------------------Vertex Shader------------------------------- */

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

#else
namespace shader::textured
{
#endif

struct MaterialData
{
    float4 diffuseColor;
    float3 specularColor;
    FLOAT3_PADDING(0);
    float3 emissiveColor;
    FLOAT3_PADDING(1);
    float shininess;
};

#ifdef __cplusplus
}
#else

struct Material
{
    SamplerState sampler;
    Texture2D<float4> diffuseTexture;
    Texture2D<float3> emissiveTexture;
    Texture2D<float3> normalTexture;
    ConstantBuffer<MaterialData> data;
}
ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    float4 texColor = material.data.diffuseColor * material.diffuseTexture.Sample(material.sampler, input.uv);

    if (texColor.a < 0.5)
        discard;

    float3 normalMap = material.normalTexture.Sample(material.sampler, input.uv) * 2.0 - 1.0;
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.bitangent);
    float3 N = normalize(input.normal);

    phong::FragmentContext fragCtx;
    fragCtx.position      = input.pos;
    fragCtx.normal        = normalize(mul(normalMap, float3x3(T, B, N)));
    fragCtx.cameraDir     = normalize(sceneData.cameraPosition - input.pos);
    fragCtx.diffuseColor  = texColor.rgb;
    fragCtx.specularColor = material.data.specularColor;
    fragCtx.shininess     = material.data.shininess;

    float3 finalColor = sceneData.ambientLightColor * fragCtx.diffuseColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += phong::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += phong::evalPointLight(fragCtx, sceneData.pointLights[i]);

    finalColor += material.data.emissiveColor * material.emissiveTexture.Sample(material.sampler, input.uv);

    return float4(saturate(finalColor), texColor.a);
}

#endif
#endif
