/*
 * ---------------------------------------------------
 * scop.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/12/15 21:52:13
 * ---------------------------------------------------
 */

#ifndef SCOP_SLANG
#define SCOP_SLANG

#include "shared_types.h"

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
    float3 normal;
};

struct VSOutput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float3 normal;
    float2 uvX;
    float2 uvY;
    float2 uvZ;
    float3 triW;
};

[shader("vertex")]
VSOutput vertexMain(Vertex input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix);

    float3 an = abs(normalize(input.normal));
    float sum = an.x + an.y + an.z + 1e-6f;
    float3 w = an / sum;

    float2 uvX = input.pos.yz;
    float2 uvY = input.pos.xz;
    float2 uvZ = input.pos.xy;

    VSOutput output;
    output.pos     = worldPos.xyz;
    output.clipPos = mul(worldPos, vpMatrix);
    output.normal  = normalize(mul(input.normal, (float3x3)modelMatrix));
    output.uvX     = uvX;
    output.uvY     = uvY;
    output.uvZ     = uvZ;
    output.triW    = w;

    return output;
}

/* -------------------------Vertex Shader------------------------------- */

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

#else
namespace shader::scop
{
#endif

struct MaterialData
{
    float4 colorPalette[6];
    float textureStrength;
};

#ifdef __cplusplus
} // namespace shader::scop
#else

struct Material
{
    SamplerState sampler;
    Texture2D<float4> diffuseTexture;
    ConstantBuffer<MaterialData> data;
}
ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input, uint primID : SV_PrimitiveID) : SV_TARGET
{
    float4 texX = material.diffuseTexture.Sample(material.sampler, input.uvX);
    float4 texY = material.diffuseTexture.Sample(material.sampler, input.uvY);
    float4 texZ = material.diffuseTexture.Sample(material.sampler, input.uvZ);
    float4 triTex = texX * input.triW.x + texY * input.triW.y + texZ * input.triW.z;

    float4 fragDiffuse = lerp(
        material.data.colorPalette[primID % 6],
        triTex,
        material.data.textureStrength
    );

    if (fragDiffuse.a < 0.5)
        discard;

    phong::FragmentContext fragCtx;
    fragCtx.position      = input.pos;
    fragCtx.normal        = normalize(input.normal);
    fragCtx.cameraDir     = normalize(sceneData.cameraPosition - input.pos);
    fragCtx.diffuseColor  = fragDiffuse.rgb;
    fragCtx.specularColor = float3(0.0f, 0.0f, 0.0f);
    fragCtx.shininess     = 1.0f;

    float3 finalColor = sceneData.ambientLightColor * fragCtx.diffuseColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += phong::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += phong::evalPointLight(fragCtx, sceneData.pointLights[i]);

    return float4(saturate(finalColor), fragDiffuse.a);
}

#endif
#endif
