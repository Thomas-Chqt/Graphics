#include "shader.h"

import utils;

namespace shader::brdf
{

public float distributionGGX(float NdotH, float a)
{
    const float a2     = a*a;
    const float NdotH2 = NdotH*NdotH;

    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = float.getPi() * denom * denom;

    return a2 / denom;
}

public float3 importanceSampleGGX(float2 xi, float a, float3x3 TBN)
{
    const float phi = 2.0 * float.getPi() * xi.x;
    const float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a*a - 1.0) * xi.y));
    const float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    const float3 tangentH = float3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );
    return normalize(mul(tangentH, TBN));
}

float geometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

public float geometrySmith(float NdotV, float NdotL, float k)
{
    float ggx1 = geometrySchlickGGX(NdotV, k);
    float ggx2 = geometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

float kDirect(float a)
{
    float ap1 = a + 1;
    return (ap1*ap1) / 8;
}

public float kIBL(float a)
{
    return (a*a) / 2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

public float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

public float3 cookTorranceBRDF(
    float3 baseColor, float metallic, float roughness,
    float NdotH, float cosTheta, float3 F0, float NdotV, float NdotL
)
{
    float  D = distributionGGX(NdotH, roughness*roughness);
    float3 F = fresnelSchlick(cosTheta, F0);
    float  G = geometrySmith(NdotV, NdotL, kDirect(roughness));

    float3 specular = (D*F*G) / max(4.0 * NdotV * NdotL, 0.0001);

    float3 kS = F;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - metallic; // remove diffuse from metals

    return kD * baseColor / float.getPi() + specular;
}

}
