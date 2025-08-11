/*
 * ---------------------------------------------------
 * mc_cube.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/25 09:18:21
 * ---------------------------------------------------
 */

struct VSInput
{
    float3 pos;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 modelPos : MODEL_POS;
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
    output.modelPos = input.pos;
    return output;
}

struct Material
{
    ConstantBuffer<float4> color;
}

ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    float dist = length(input.modelPos);
    float brightness = lerp(1.0, 0.5, saturate(dist / 0.866));
    return material.color * brightness;
}
