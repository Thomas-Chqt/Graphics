#include "shader.h"

import brdf;

namespace shader::ibl
{

public float3 evaluateIBL(
    float3 baseColor, float metallic, float roughness,
    float NdotV, float3 F0,
    float3 irradiance, float3 prefilteredColor, float2 environmentBrdf
)
{
    float3 kSa = brdf::fresnelSchlickRoughness(NdotV, F0, roughness);
    float3 kDa = float3(1.0) - kSa;
    kDa *= 1.0 - metallic; // remove diffuse from metals

    float3 diffuse = irradiance * baseColor / float.getPi();
    float3 specular = prefilteredColor * (F0 * environmentBrdf.x + environmentBrdf.y);

    return kDa * diffuse + specular;
}

}
