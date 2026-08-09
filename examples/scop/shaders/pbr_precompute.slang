#include "shader.h"

import utils;
import brdf;

namespace shader::pbr
{

struct EnvironmentCubeMapResources
{
    Texture2D<float4> equirectangularMap;
    SamplerState sampler;

    [format("rgba16f")]
    RWTexture2DArray<float4> environmentCubeMap;
};

[shader("compute")]
[numthreads(8, 8, 1)]
void equirectangularToCubemap(uint3 id : SV_DispatchThreadID,
    uniform ParameterBlock<EnvironmentCubeMapResources> resources
)
{
    uint width;
    uint height;
    uint layerCount;
    resources.environmentCubeMap.GetDimensions(width, height, layerCount);

    if (id.x >= width || id.y >= height || id.z >= layerCount)
        return;

    const float2 faceUV = (float2(id.xy) + 0.5) / float2(width, height);
    const float3 sampleVector = cubeDirection(id.z, faceUV * 2.0 - 1.0);

    const float2 uv = float2(
        atan2(sampleVector.z, sampleVector.x) / (2.0 * float.getPi()) + 0.5,
        acos(clamp(sampleVector.y, -1.0, 1.0)) / float.getPi()
    );
    resources.environmentCubeMap[id] = resources.equirectangularMap.SampleLevel(resources.sampler, uv, 0.0);
}

struct IrradianceResources
{
    TextureCube<float4> environmentCubeMap;
    SamplerState sampler;

    [format("rgba16f")]
    RWTexture2DArray<float4> irradianceCubeMap;
};

float3 cosineSampleHemisphere(float2 xi, float3x3 TBN)
{
    const float phi = 2.0 * float.getPi() * xi.x;
    const float sinTheta = sqrt(xi.y);
    const float cosTheta = sqrt(1.0 - xi.y);

    const float3 localL = float3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );
    return normalize(mul(localL, TBN));
}

[shader("compute")]
[numthreads(8, 8, 1)]
void convolveDiffuseIrradiance(uint3 id : SV_DispatchThreadID,
    uniform ParameterBlock<IrradianceResources> resources
)
{
    constexpr uint sampleCount = 1024;

    uint width;
    uint height;
    uint layerCount;
    resources.irradianceCubeMap.GetDimensions(width, height, layerCount);

    if (id.x >= width || id.y >= height || id.z >= layerCount)
        return;

    const float2 faceUV = (float2(id.xy) + 0.5) / float2(width, height);
    const float3 N = cubeDirection(id.z, faceUV * 2.0 - 1.0);
    const float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 T = normalize(cross(up, N));
    const float3 B = cross(N, T);
    const float3x3 TBN = float3x3(T, B, N);

    uint environmentWidth;
    uint environmentHeight;
    uint environmentMipCount;
    resources.environmentCubeMap.GetDimensions(0, environmentWidth, environmentHeight, environmentMipCount);

    const float sourceTexelSolidAngle = 4.0 * float.getPi() / (6.0 * float(environmentWidth) * float(environmentHeight));

    float3 irradiance = float3(0.0);
    for (uint sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        const float2 xi = hammersley(sampleIndex, sampleCount);
        const float3 L = cosineSampleHemisphere(xi, TBN);

        const float NdotL = max(dot(N, L), 0.0);
        const float pdf = NdotL / float.getPi();
        const float sampleSolidAngle = 1.0 / max(float(sampleCount) * pdf, 0.0001);
        const float sourceMip = clamp(
            0.5 * log2(sampleSolidAngle / sourceTexelSolidAngle),
            0.0,
            float(environmentMipCount - 1)
        );

        irradiance += resources.environmentCubeMap.SampleLevel(resources.sampler, L, sourceMip).rgb;
    }
    irradiance *= float.getPi() / float(sampleCount);

    resources.irradianceCubeMap[id] = float4(irradiance, 1.0);
}

struct PrefilterResources
{
    TextureCube<float4> environmentCubeMap;
    SamplerState sampler;

    [format("rgba16f")]
    RWTexture2DArray<float4> prefilteredEnvironmentCubeMap;
};

[shader("compute")]
[numthreads(8, 8, 1)]
void prefilterEnvironment(uint3 id : SV_DispatchThreadID,
    uniform ParameterBlock<PrefilterResources> resources,
    uniform ParameterBlock<ConstantBuffer<float>> roughness
)
{
    constexpr uint sampleCount = 1024;

    uint width;
    uint height;
    uint layerCount;
    resources.prefilteredEnvironmentCubeMap.GetDimensions(width, height, layerCount);

    if (id.x >= width || id.y >= height || id.z >= layerCount)
        return;

    const float2 faceUV = (float2(id.xy) + 0.5) / float2(width, height);
    const float3 N = cubeDirection(id.z, faceUV * 2.0 - 1.0);
    const float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 T = normalize(cross(up, N));
    const float3 B = cross(N, T);
    const float3x3 TBN = float3x3(T, B, N);
    const float roughness2 = roughness*roughness;
    const float3 V = N;

    uint environmentWidth;
    uint environmentHeight;
    uint environmentMipCount;
    resources.environmentCubeMap.GetDimensions(0, environmentWidth, environmentHeight, environmentMipCount);

    const float sourceTexelSolidAngle = 4.0 * float.getPi() / (6.0 * float(environmentWidth) * float(environmentHeight));

    float3 prefilteredColor = float3(0.0);
    float totalWeight = 0.0;
    for (uint sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        const float2 xi = hammersley(sampleIndex, sampleCount);
        const float3 H = brdf::importanceSampleGGX(xi, roughness2, TBN);

        const float HdotV = max(dot(H, V), 0.0);
        const float3 L = normalize(2.0 * HdotV * H - V);

        const float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            float sourceMip = 0.0;
            if (roughness > 0.0)
            {
                const float NdotH = max(dot(N, H), 0.0);
                const float distribution = brdf::distributionGGX(NdotH, roughness2);
                const float pdf = distribution * NdotH / max(4.0 * HdotV, 0.0001);
                const float sampleSolidAngle = 1.0 / max(float(sampleCount) * pdf, 0.0001);
                sourceMip = 0.5 * log2(sampleSolidAngle / sourceTexelSolidAngle);
            }

            prefilteredColor += resources.environmentCubeMap.SampleLevel(resources.sampler, L, clamp(sourceMip, 0.0, float(environmentMipCount - 1))).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor /= totalWeight;

    resources.prefilteredEnvironmentCubeMap[id] = float4(prefilteredColor, 1.0);
}

struct BrdfIntegrationResources
{
    [format("rg16f")]
    RWTexture2D<float2> brdfLut;
};

[shader("compute")]
[numthreads(8, 8, 1)]
void integrateBrdf(uint3 id : SV_DispatchThreadID,
    uniform ParameterBlock<BrdfIntegrationResources> resources
)
{
    constexpr uint sampleCount = 1024;

    uint width;
    uint height;
    resources.brdfLut.GetDimensions(width, height);

    if (id.x >= width || id.y >= height)
        return;

    const float2 uv = (float2(id.xy) + 0.5) / float2(width, height);
    const float NdotV = uv.x;
    const float roughness = uv.y;

    const float3 N = float3(0.0, 0.0, 1.0);
    const float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 T = normalize(cross(up, N));
    const float3 B = cross(N, T);
    const float3x3 TBN = float3x3(T, B, N);
    const float roughness2 = roughness*roughness;
    const float3 V = float3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);

    float scale = 0.0;
    float bias = 0.0;

    for (uint sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        const float2 xi = hammersley(sampleIndex, sampleCount);
        const float3 H = brdf::importanceSampleGGX(xi, roughness2, TBN);

        const float HdotV = max(dot(H, V), 0.0);
        const float3 L = normalize(2.0 * HdotV * H - V);

        const float NdotL = max(L.z, 0.0);
        if (NdotL > 0.0)
        {
            const float NdotH = max(H.z, 0.0);
            const float geometry = brdf::geometrySmith(NdotV, NdotL, brdf::kIBL(roughness));
            const float geometryVisibility = geometry * HdotV / (NdotH * NdotV);
            const float fresnel = pow(1.0 - HdotV, 5.0);

            scale += (1.0 - fresnel) * geometryVisibility;
            bias += fresnel * geometryVisibility;
        }
    }

    resources.brdfLut[id.xy] = float2(scale, bias) / float(sampleCount);
}

}
