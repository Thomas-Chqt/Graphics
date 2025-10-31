/*
 * ---------------------------------------------------
 * common_vertex.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/05 10:53:35
 * ---------------------------------------------------
 */

module common_vertex;

struct VSInput
{
    float3 pos;
    float2 uv;
    float3 normal;
};

struct VSOutput
{
    float3 pos;
    float4 clipPos : SV_Position;
    float2 uv;
    float3 normal;
};

struct _VPMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_VPMatrix> vpMatrix;
#define vpMatrix vpMatrix.data

struct _ModelMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_ModelMatrix> modelMatrix;
#define modelMatrix modelMatrix.data

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix);

    VSOutput output;
    output.pos = worldPos.xyz;
    output.clipPos = mul(worldPos, vpMatrix);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)modelMatrix);
    return output;
}
