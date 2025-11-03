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

#include "shared_types.h"

#ifndef __cplusplus

import SceneData;
import common_vertex;
import phong_shading;

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
