/*
 * ---------------------------------------------------
 * flat_color.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/23 09:12:21
 * ---------------------------------------------------
 */

static const int LIGHT_TYPE_DIRECTIONAL = 0;
static const int LIGHT_TYPE_POINT = 1;

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

struct VpMatrix
{
    ConstantBuffer<float4x4> mat;
}
ParameterBlock<VpMatrix> vpMatrix;

struct ModelMatrix
{
    ConstantBuffer<float4x4> mat;
}
ParameterBlock<ModelMatrix> modelMatrix;

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix.mat);

    VSOutput output;
    output.pos = worldPos.xyz;
    output.clipPos = mul(worldPos, vpMatrix.mat);
    output.normal = mul(input.normal, (float3x3)modelMatrix.mat);
    return output;
}

struct DirectionalLight
{
    float4 position;
    float4 color;
};

struct PointLight
{
    float4 position;
    float4 color;
    float attenuation;
};

struct SceneDataCB
{
    float4 cameraPosition;
    float4 ambientLightColor;

    int directionalLightCount;
    DirectionalLight directionalLights[8];

    int pointLightCount;
    PointLight pointLights[8];
};

struct SceneData
{
    ConstantBuffer<SceneDataCB> data;
}
ParameterBlock<SceneData> sceneData;

struct Material
{
    ConstantBuffer<float3> diffuseColor;
    ConstantBuffer<float>  shininess;
    ConstantBuffer<float>  specular;
}
ParameterBlock<Material> material;

float3 evalDirectionalLight(DirectionalLight light, Material material, float3 fragNormal, float3 cameraDir)
{
    float3 lightDir = normalize(light.position.xyz);
    float3 halfwayDir = normalize(lightDir + cameraDir);

    float diffuseFactor = max(dot(fragNormal, lightDir), 0.0f);
    float specularFactor = pow(max(dot(fragNormal, halfwayDir), 0.0f), material.shininess) * material.specular;

    float3 diffuse  = material.diffuseColor.rgb * light.color.rgb * diffuseFactor;
    float3 specular =                             light.color.rgb * specularFactor;

    return diffuseFactor + specular;
}

float3 evalPointLight(PointLight light, Material material, float3 fragPos, float3 fragNormal, float3 cameraDir)
{
    float3 lightDir = normalize(light.position.xyz - fragPos);
    float3 halfwayDir = normalize(lightDir + cameraDir);

    float diffuseFactor = max(dot(fragNormal, lightDir), 0.0f);
    float specularFactor = pow(max(dot(fragNormal, halfwayDir), 0.0f), material.shininess) * material.specular;

    float3 diffuse  = material.diffuseColor.rgb * light.color.rgb * diffuseFactor;
    float3 specular =                             light.color.rgb * specularFactor;

    return diffuse + specular;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    float3 fragNormal = normalize(input.normal);
    float3 cameraDir = normalize(sceneData.data.cameraPosition.xyz - input.pos);

    float3 finalColor = sceneData.data.ambientLightColor.rgb;
    for (int i = 0; i < sceneData.data.directionalLightCount; ++i)
        finalColor += evalDirectionalLight(sceneData.data.directionalLights[i], material, fragNormal, cameraDir);
    for (int i = 0; i < sceneData.data.pointLightCount; ++i)
        finalColor += evalPointLight(sceneData.data.pointLights[i], material, input.pos, fragNormal, cameraDir);

    return saturate(float4(finalColor, 1.0f));
}
