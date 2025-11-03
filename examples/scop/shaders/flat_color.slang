/*
 * ---------------------------------------------------
 * flat_color.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/23 09:12:21
 * ---------------------------------------------------
 */

#ifndef FLAT_COLOR_SLANG
#define FLAT_COLOR_SLANG

#include "shared_types.h"

#ifndef __cplusplus

import SceneData;
import common_vertex;
import phong_shading;

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

#else
namespace shader::flat_color
{
#endif

struct MaterialData
{
    float4 diffuseColor;
    float3 specularColor;
    FLOAT3_PADDING(0);
    float shininess;
};

#ifdef __cplusplus
}
#else

struct Material { ConstantBuffer<MaterialData> data; };
ParameterBlock<Material> material;
#define material material.data

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    if (material.diffuseColor.a < 0.5)
        discard;

    phong::FragmentContext fragCtx;
    fragCtx.position      = input.pos;
    fragCtx.normal        = normalize(input.normal);
    fragCtx.cameraDir     = normalize(sceneData.cameraPosition - input.pos);
    fragCtx.diffuseColor  = material.diffuseColor.rgb;
    fragCtx.specularColor = material.specularColor;
    fragCtx.shininess     = material.shininess;

    float3 finalColor = sceneData.ambientLightColor * fragCtx.diffuseColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += phong::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += phong::evalPointLight(fragCtx, sceneData.pointLights[i]);

    return float4(saturate(finalColor), material.diffuseColor.a);
}

#endif
#endif
