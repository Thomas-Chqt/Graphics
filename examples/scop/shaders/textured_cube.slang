/*
 * ---------------------------------------------------
 * textured_cube.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/25 09:18:21
 * ---------------------------------------------------
 */

import scene_data;
import phong_shading;

struct VSInput
{
    float3 pos;
    float3 normal;
};

struct VSOutput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float3 normal;
    float3 cubeDir;
};

struct _VPMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_VPMatrix> vpMatrix;
#define vpMatrix vpMatrix.data

struct _ModelMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_ModelMatrix> modelMatrix;
#define modelMatrix modelMatrix.data

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix);

    VSOutput output;
    output.pos = worldPos.xyz;
    output.clipPos = mul(worldPos, vpMatrix);
    output.normal = mul(input.normal, (float3x3)modelMatrix);
    output.cubeDir = input.pos; // Use input position directly as cube direction
    return output;
}

struct _SceneData { ConstantBuffer<SceneData> data; };
ParameterBlock<_SceneData> sceneData;
#define sceneData sceneData.data

struct Material
{
    TextureCube<float3> texture;
    SamplerState sampler;
}
ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    // Use the cube direction directly (vertices are already centered on 0)
    float3 sampleDir = normalize(input.cubeDir);
    
    phong::FragmentContext fragCtx;
    fragCtx.position = input.pos;
    fragCtx.normal = normalize(input.normal);
    fragCtx.cameraDir = normalize(sceneData.cameraPosition - input.pos);
    fragCtx.diffuseColor = material.texture.Sample(material.sampler, normalize(input.cubeDir));
    fragCtx.shininess = 32;
    fragCtx.specularIntensity = 0;

    float3 finalColor = fragCtx.diffuseColor * sceneData.ambientLightColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += phong::evalDirectionalLight(fragCtx, sceneData.directionalLights[i]);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += phong::evalPointLight(fragCtx, sceneData.pointLights[i]);

    return saturate(float4(finalColor, 1.0f));
}
