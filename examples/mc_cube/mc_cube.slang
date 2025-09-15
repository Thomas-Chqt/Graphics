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
    float2 uv;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 cubeDir : CUBE_DIR;
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
    output.cubeDir = input.pos; // Use input position directly as cube direction
    return output;
}

struct Material
{
    TextureCube<float4> texture;
    SamplerState sampler;
}

ParameterBlock<Material> material;

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    // Use the cube direction directly (vertices are already centered on 0)
    float3 sampleDir = normalize(input.cubeDir);
    return material.texture.Sample(material.sampler, sampleDir);
}
