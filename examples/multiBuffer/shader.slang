/*
 * ---------------------------------------------------
 * shader.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/12 14:30:00
 * ---------------------------------------------------
 */

struct VSInput {
    float2 inPosition;
    float3 inColor;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
};

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.inPosition, 0.0, 1.0);
    output.color = input.inColor;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput vertIn) : SV_TARGET
{
    return float4(vertIn.color, 1.0);
}
