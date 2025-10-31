/*
 * ---------------------------------------------------
 * textured.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/30 08:54:54
 * ---------------------------------------------------
 */

import scene_data;
import phong_shading;

struct FSInput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float2 uv;
    float3 normal;
};

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

struct MaterialData
{
    float3 diffuseColor;
#ifdef __SPIRV__
    float _padding0;
#endif
    float3 specularColor;
#ifdef __SPIRV__
    float _padding1;
#endif
    float shininess;
};

struct Material
{
    SamplerState sampler;
    Texture2D<float3> diffuseTexture;
    ConstantBuffer<MaterialData> data;
}
ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(FSInput input) : SV_TARGET
{
    phong::FragmentContext fragCtx;
    fragCtx.position      = input.pos;
    fragCtx.normal        = normalize(input.normal);
    fragCtx.cameraDir     = normalize(sceneData.cameraPosition - input.pos);
    fragCtx.diffuseColor  = material.data.diffuseColor * material.diffuseTexture.Sample(material.sampler, input.uv);
    fragCtx.specularColor = material.data.specularColor;
    fragCtx.shininess     = material.data.shininess;

    float3 finalColor = sceneData.ambientLightColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += phong::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += phong::evalPointLight(fragCtx, sceneData.pointLights[i]);

    return saturate(float4(finalColor, 1.0f));
}
