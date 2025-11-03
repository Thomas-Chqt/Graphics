/*
 * ---------------------------------------------------
 * phong_shading.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/05 15:01:11
 * ---------------------------------------------------
 */

module phong_shading;

import Light;

namespace phong
{

public struct FragmentContext
{
    public float3 position;
    public float3 normal;
    public float3 cameraDir;
    public float3 diffuseColor;
    public float3 specularColor;
    public float  shininess;
};

public float3 evalDirectionalLight(FragmentContext fragCtx, DirectionalLight light)
{
    float3 lightDir   = normalize(light.position);
    float3 halfwayDir = normalize(lightDir + fragCtx.cameraDir);

    float3 diffuse  = fragCtx.diffuseColor  * light.color *     max(dot(fragCtx.normal, lightDir  ), 0.0f);
    float3 specular = fragCtx.specularColor * light.color * pow(max(dot(fragCtx.normal, halfwayDir), 0.0f), fragCtx.shininess);

    return diffuse + specular;
}

public float3 evalPointLight(FragmentContext fragCtx, PointLight light)
{
    float3 lightDir   = normalize(light.position - fragCtx.position);
    float3 halfwayDir = normalize(lightDir + fragCtx.cameraDir);

    float3 diffuse  = fragCtx.diffuseColor  * light.color *     max(dot(fragCtx.normal, lightDir  ), 0.0f);
    float3 specular = fragCtx.specularColor * light.color * pow(max(dot(fragCtx.normal, halfwayDir), 0.0f), fragCtx.shininess);

    return diffuse + specular;
}

}
