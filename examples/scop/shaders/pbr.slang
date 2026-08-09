module pbr;

import Light;
import brdf;

namespace pbr
{

public struct FragmentContext
{
    public float3 position;
    public float3 normal;
    public float3 cameraDir;
    public float3 baseColor;
    public float metallic;
    public float roughness;
};

float3 evaluateLight(FragmentContext fragCtx, float3 lightDir, float3 radiance)
{
    const float3 halfwayDir = normalize(fragCtx.cameraDir + lightDir);

    const float NdotV = max(dot(fragCtx.normal, fragCtx.cameraDir), 0.0);
    const float NdotL = max(dot(fragCtx.normal, lightDir), 0.0);
    const float NdotH = max(dot(fragCtx.normal, halfwayDir), 0.0);
    const float HdotV = max(dot(halfwayDir, fragCtx.cameraDir), 0.0);

    const float3 F0 = lerp(float3(0.04), fragCtx.baseColor, fragCtx.metallic);

    return radiance
           * shader::brdf::cookTorranceBRDF(
               fragCtx.baseColor,
               fragCtx.metallic,
               fragCtx.roughness,
               NdotH,
               HdotV,
               F0,
               NdotV,
               NdotL
           )
           * NdotL;
}

public float3 evalDirectionalLight(FragmentContext fragCtx, DirectionalLight light)
{
    const float3 lightDir = normalize(light.position);
    return evaluateLight(fragCtx, lightDir, light.color);
}

public float3 evalPointLight(FragmentContext fragCtx, PointLight light)
{
    const float3 lightVector = light.position - fragCtx.position;
    const float distance = length(lightVector);
    const float3 lightDir = lightVector / distance;
    const float attenuation = 1.0 / (distance * distance);
    const float3 radiance = light.color * attenuation;

    return evaluateLight(fragCtx, lightDir, radiance);
}

}
