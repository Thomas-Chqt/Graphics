#include "shader.h"

SLANG_MODULE_IMP(utils);
SLANG_MODULE_IMP(brdf);
SLANG_MODULE_IMP(ibl);

namespace shader::pbr
{

struct Uniforms
{
    float4x4 modelMatrix;
    float4x4 vpMatrix;

    float4 albedo;
    float metallic;
    float roughness;
    float exposure;
    float _padding0;

    float3 lightPosition;  FLOAT3_PADDING(0);
    float3 lightColor;     FLOAT3_PADDING(1);

    float3 cameraPosition; FLOAT3_PADDING(2);
};

/* -------------------------Vertex Shader------------------------------- */

#ifndef __cplusplus

struct Textures
{
    TextureCube<float4> irradianceCubeMap;
    TextureCube<float4> prefilteredEnvironmentCubeMap;
    Texture2D<float2> brdfLut;
    SamplerState sampler;
};

struct Vertex
{
    float3 pos;
    float3 normal;
};

struct VSOutput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float3 normal;
};

ParameterBlock<ConstantBuffer<Uniforms>> uniforms;
ParameterBlock<Textures> textures;

[shader("vertex")]
VSOutput vertexMain(Vertex input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), uniforms.modelMatrix);

    VSOutput output;
    output.pos     = worldPos.xyz;
    output.clipPos = mul(worldPos, uniforms.vpMatrix);
    output.normal  = mul(input.normal, (float3x3)uniforms.modelMatrix);
    return output;
}

/* -------------------------Fragment Shader------------------------------- */

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target0
{
    const float3 N = normalize(input.normal);
    const float3 V = normalize(uniforms.cameraPosition - input.pos);
    const float NdotV = max(dot(N, V), 0.0);

    // surface reflection at zero incidence.
    // 0.04 for dielectric and albedo value for metals
    const float3 F0 = lerp(float3(0.04), uniforms.albedo.rgb, uniforms.metallic);

    float3 Lo = float3(0.0);
    //for each lights:
    //{
        const float3 L = normalize(uniforms.lightPosition - input.pos);
        const float NdotL = max(dot(N, L), 0.0);

        const float3 H = normalize(V + L);
        const float NdotH = max(dot(N, H), 0.0);
        const float cosTheta = max(dot(H, V), 0.0);

        const float distance = length(uniforms.lightPosition - input.pos);
        const float attenuation = 1.0 / (distance * distance);
        const float3 radiance = uniforms.lightColor * attenuation;

        Lo += brdf::cookTorranceBRDF(uniforms.albedo.rgb, uniforms.metallic, uniforms.roughness, NdotH, cosTheta, F0, NdotV, NdotL)
            * radiance
            * NdotL;
    //}

    uint prefilteredWidth;
    uint prefilteredHeight;
    uint prefilteredMipCount;
    textures.prefilteredEnvironmentCubeMap.GetDimensions(0, prefilteredWidth, prefilteredHeight, prefilteredMipCount);

    const float3 irradiance = textures.irradianceCubeMap.Sample(textures.sampler, N).rgb;

    const float3 R = reflect(-V, N);
    const float mipLevel = uniforms.roughness * float(prefilteredMipCount - 1);
    const float3 prefilteredColor = textures.prefilteredEnvironmentCubeMap.SampleLevel(textures.sampler, R, mipLevel).rgb;

    const float2 environmentBrdf = textures.brdfLut.SampleLevel(textures.sampler, float2(NdotV, uniforms.roughness), 0.0).rg;

    const float3 ambient = ibl::evaluateIBL(
        uniforms.albedo.rgb, uniforms.metallic, uniforms.roughness,
        NdotV, F0,
        irradiance, prefilteredColor, environmentBrdf
    );

    const float3 color = Lo + ambient;
    return float4(acesToneMapping(color * uniforms.exposure), uniforms.albedo.w);
}

#endif // ifndef _cplusplus

} // namespace shader::pbr
