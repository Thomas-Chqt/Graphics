/*
 * ---------------------------------------------------
 * vertex.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/24 07:33:08
 * ---------------------------------------------------
 */

static float2 positions[3] = {
    float2( 0.0, -0.5),
    float2( 0.5,  0.5),
    float2(-0.5,  0.5)
};

static float3 colors[3] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

struct VertexStageOutput
{
    float3 color    : COLOR;
    float4 position : SV_Position;
};


[shader("vertex")]
VertexStageOutput vertexMain(uint vertexID : SV_VertexID)
{
    VertexStageOutput output;

    output.color = colors[vertexID];
    output.position = float4(positions[vertexID], 0.0, 1.0);

    return output;
}

[shader("fragment")]
float4 fragmentMain(float3 color : COLOR) : SV_Target
{
    return float4(color, 1.0);
}
