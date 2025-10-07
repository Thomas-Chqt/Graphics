/*
 * ---------------------------------------------------
 * phong_shading.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/05 15:01:11
 * ---------------------------------------------------
 */

module phong_shading;

import light;

namespace phong
{

public struct FragmentContext
{
    public float3 position;
    public float3 normal;
    public float3 cameraDir;
    public float3 diffuseColor;
    public float shininess;
    public float specularIntensity;
};

public float3 evalDirectionalLight(FragmentContext fragCtx, DirectionalLight light)
{
    float3 lightDir = normalize(light.position);
    float3 halfwayDir = normalize(lightDir + fragCtx.cameraDir);

    float diffuseFactor = max(dot(fragCtx.normal, lightDir), 0.0f);
    float specularFactor = pow(max(dot(fragCtx.normal, halfwayDir), 0.0f), fragCtx.shininess) * fragCtx.specularIntensity;

    float3 diffuse  = fragCtx.diffuseColor * light.color * diffuseFactor;
    float3 specular = float3(1, 1, 1)      * light.color * specularFactor;

    return diffuse + specular;
}

public float3 evalPointLight(FragmentContext fragCtx, PointLight light)
{
    float3 lightDir = normalize(light.position - fragCtx.position);
    float3 halfwayDir = normalize(lightDir + fragCtx.cameraDir);

    float diffuseFactor = max(dot(fragCtx.normal, lightDir), 0.0f);
    float specularFactor = pow(max(dot(fragCtx.normal, halfwayDir), 0.0f), fragCtx.shininess) * fragCtx.specularIntensity;

    float3 diffuse  = fragCtx.diffuseColor * light.color * diffuseFactor;
    float3 specular = float3(1, 1, 1)      * light.color * specularFactor;

    return diffuse + specular;
}

}
