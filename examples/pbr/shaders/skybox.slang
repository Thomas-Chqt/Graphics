#include "shader.h"

SLANG_MODULE_IMP(utils);

namespace shader::skybox
{

PUSH_CONSTANT
{
    float4x4 viewProjection;
    float exposure;
    FLOAT1_PADDING(0);
};

#ifndef __cplusplus

static const float3 vertices[36] = {
    float3(-1.0, -1.0, -1.0), float3( 1.0,  1.0, -1.0), float3( 1.0, -1.0, -1.0),
    float3(-1.0, -1.0, -1.0), float3(-1.0,  1.0, -1.0), float3( 1.0,  1.0, -1.0),

    float3(-1.0, -1.0,  1.0), float3( 1.0, -1.0,  1.0), float3( 1.0,  1.0,  1.0),
    float3(-1.0, -1.0,  1.0), float3( 1.0,  1.0,  1.0), float3(-1.0,  1.0,  1.0),

    float3(-1.0, -1.0, -1.0), float3(-1.0, -1.0,  1.0), float3(-1.0,  1.0,  1.0),
    float3(-1.0, -1.0, -1.0), float3(-1.0,  1.0,  1.0), float3(-1.0,  1.0, -1.0),

    float3( 1.0, -1.0, -1.0), float3( 1.0,  1.0, -1.0), float3( 1.0,  1.0,  1.0),
    float3( 1.0, -1.0, -1.0), float3( 1.0,  1.0,  1.0), float3( 1.0, -1.0,  1.0),

    float3(-1.0, -1.0, -1.0), float3( 1.0, -1.0, -1.0), float3( 1.0, -1.0,  1.0),
    float3(-1.0, -1.0, -1.0), float3( 1.0, -1.0,  1.0), float3(-1.0, -1.0,  1.0),

    float3(-1.0,  1.0, -1.0), float3(-1.0,  1.0,  1.0), float3( 1.0,  1.0,  1.0),
    float3(-1.0,  1.0, -1.0), float3( 1.0,  1.0,  1.0), float3( 1.0,  1.0, -1.0)
};

struct VSOutput
{
    float4 clipPosition : SV_Position;
    float3 direction;
};

struct SkyboxResources
{
    TextureCube<float4> environment;
    SamplerState environmentSampler;
};

ParameterBlock<SkyboxResources> resources;

[shader("vertex")]
VSOutput vertexMain(uint vertexID : SV_VertexID)
{
    float3 vertex = vertices[vertexID];
    float4 position = mul(float4(vertex, 1.0), viewProjection);

    VSOutput output;
    output.clipPosition = position.xyww;
    output.direction = vertex;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target0
{
    float3 color = resources.environment.Sample(resources.environmentSampler, normalize(input.direction)).rgb;
    return float4(acesToneMapping(color * exposure), 1.0);
}

#endif

}
