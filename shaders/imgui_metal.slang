/*
 * ---------------------------------------------------
 * imgui_metal.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/02 07:25:19
 * ---------------------------------------------------
 */

struct VertexIn {
    float2 position;
    float2 texCoords;
    uint color;
};

struct VertexOut {
    float4 position : SV_Position;
    float2 texCoords;
    float4 color;
};

struct Uniforms
{
    float4x4 projectionMatrix;
};

ParameterBlock<Uniforms> uniforms;

[shader("vertex")]
VertexOut vertex_main(VertexIn in)
{
    VertexOut out;
    out.position = mul(float4(in.position, 0, 1), uniforms.projectionMatrix);
    out.texCoords = in.texCoords;
    out.color = float4((in.color >>  0) & 0xFF,
                       (in.color >>  8) & 0xFF,
                       (in.color >> 16) & 0xFF,
                       (in.color >> 24) & 0xFF) / 255.0;
    return out;
}

Texture2D<half4> texture; 
SamplerState linearSampler;

[shader("fragment")]
half4 fragment_main(VertexOut in)
{
    half4 texColor = texture.Sample(linearSampler, in.texCoords);
    return half4(in.color) * texColor;
}
