/*
 * ---------------------------------------------------
 * flat_color.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/23 09:12:21
 * ---------------------------------------------------
 */

struct VSInput
{
    float3 pos;
};

struct VSOutput
{
    float4 pos : SV_Position;
};

struct VpMatrix
{
    ConstantBuffer<float4x4> mat;
}

struct ModelMatrix
{
    ConstantBuffer<float4x4> mat;
}

ParameterBlock<VpMatrix> vpMatrix;
ParameterBlock<ModelMatrix> modelMatrix;

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    float4x4 mvpMatrix = mul(modelMatrix.mat, vpMatrix.mat);
    output.pos = mul(float4(input.pos, 1.0), mvpMatrix);
    return output;
}

struct Material
{
    ConstantBuffer<float3> color;
}

ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    return float4(material.color, 1.0f);
}
