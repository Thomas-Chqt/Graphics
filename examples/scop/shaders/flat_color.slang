/*
 * ---------------------------------------------------
 * flat_color.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/23 09:12:21
 * ---------------------------------------------------
 */

#include "scop_shader.h"

struct VSInput
{
    float3 pos;
    float3 normal;
};

struct VSOutput
{
    float3   pos;
    float4   clipPos : SV_Position;
    float3   normal;
};

ParameterBlockSingleCB(float4x4, 0, vpMatrix);
#define vpMatrix vpMatrix.data

ParameterBlockSingleCB(float4x4, 1, modelMatrix);
#define modelMatrix modelMatrix.data

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix);

    VSOutput output;
    output.pos = worldPos.xyz;
    output.clipPos = mul(worldPos, vpMatrix);
    output.normal = mul(input.normal, (float3x3)modelMatrix);
    return output;
}

ParameterBlockSingleCB(SceneData, 0, sceneData);
#define sceneData sceneData.data

ParameterBlockSingleCB(FlatColorMaterial, 0, material);
#define material material.data

float3 evalDirectionalLight(DirectionalLight light, float3 fragNormal, float3 cameraDir)
{
    float3 lightDir = normalize(light.position);
    float3 halfwayDir = normalize(lightDir + cameraDir);

    float diffuseFactor = max(dot(fragNormal, lightDir), 0.0f);
    float specularFactor = pow(max(dot(fragNormal, halfwayDir), 0.0f), material.shininess) * material.specular;

    float3 diffuse  = material.diffuseColor.rgb * light.color * diffuseFactor;
    float3 specular =                             light.color * specularFactor;

    return diffuse + specular;
}

float3 evalPointLight(PointLight light, float3 fragPos, float3 fragNormal, float3 cameraDir)
{
    float3 lightDir = normalize(light.position - fragPos);
    float3 halfwayDir = normalize(lightDir + cameraDir);

    float diffuseFactor = max(dot(fragNormal, lightDir), 0.0f);
    float specularFactor = pow(max(dot(fragNormal, halfwayDir), 0.0f), material.shininess) * material.specular;

    float3 diffuse  = material.diffuseColor.rgb * light.color * diffuseFactor;
    float3 specular =                             light.color * specularFactor;

    return diffuse + specular;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    float3 fragNormal = normalize(input.normal);
    float3 cameraDir = normalize(sceneData.cameraPosition - input.pos);

    float3 finalColor = sceneData.ambientLightColor;
    for (int i = 0; i < sceneData.directionalLightCount; ++i)
        finalColor += evalDirectionalLight(sceneData.directionalLights[i], fragNormal, cameraDir);
    for (int i = 0; i < sceneData.pointLightCount; ++i)
        finalColor += evalPointLight(sceneData.pointLights[i], input.pos, fragNormal, cameraDir);

    return saturate(float4(finalColor, 1.0f));
}
