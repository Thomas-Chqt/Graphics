/*
 * ---------------------------------------------------
 * common_vertex.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/05 10:53:35
 * ---------------------------------------------------
 */

module common_vertex;

import Vertex;

public struct VSOutput
{
    public float3 pos;
    public float4 clipPos : SV_Position;
    public float2 uv;
    public float3 tangent;
    public float3 bitangent;
    public float3 normal;
};

struct _VPMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_VPMatrix> vpMatrix;
#define vpMatrix vpMatrix.data

struct _ModelMatrix { ConstantBuffer<float4x4> data; };
ParameterBlock<_ModelMatrix> modelMatrix;
#define modelMatrix modelMatrix.data

[shader("vertex")]
VSOutput vertexMain(Vertex input)
{
    float4 worldPos  = mul(float4(input.pos, 1.0), modelMatrix);
    float3 bitangent = cross(input.normal, input.tangent);

    VSOutput output;
    output.pos       = worldPos.xyz;
    output.clipPos   = mul(worldPos, vpMatrix);
    output.uv        = input.uv;
    output.tangent   = mul(input.tangent, (float3x3)modelMatrix);
    output.bitangent = mul(bitangent, (float3x3)modelMatrix);
    output.normal    = mul(input.normal,  (float3x3)modelMatrix);
    return output;
}
