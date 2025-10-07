/*
 * ---------------------------------------------------
 * flat_color.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/23 09:12:21
 * ---------------------------------------------------
 */

import scene_data;
import phong_shading;

struct FSInput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float3 normal;
};

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

struct Material
{
    float3 diffuseColor;
#ifdef __SPIRV__
    float _padding;
#endif
    float shininess;
    float specular;
};

struct _Material { ConstantBuffer<Material> data; };
ParameterBlock<_Material> material;
#define material material.data

[shader("fragment")]
float4 fragmentMain(FSInput input) : SV_TARGET
{
    phong::FragmentContext fragCtx;
    fragCtx.position = input.pos;
    fragCtx.normal = normalize(input.normal);
    fragCtx.cameraDir = normalize(sceneData.cameraPosition - input.pos);
    fragCtx.diffuseColor = material.diffuseColor;
    fragCtx.shininess = material.shininess;
    fragCtx.specularIntensity = material.specular;

    float3 finalColor = sceneData.ambientLightColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += phong::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += phong::evalPointLight(fragCtx, sceneData.pointLights[i]);

    return saturate(float4(finalColor, 1.0f));
}
