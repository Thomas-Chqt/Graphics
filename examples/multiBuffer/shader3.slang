/*
 * ---------------------------------------------------
 * shader3.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/12 14:30:00
 * ---------------------------------------------------
 */

static float2 positions[6] = {
    float2(-0.5, -0.5),
    float2( 1.0, -0.5),
    float2( 1.0,  1.0),

    float2(-0.5, -0.5),
    float2( 1.0,  1.0),
    float2(-0.5,  1.0)
};

[shader("vertex")]
float4 vertexMain(uint vertexID : SV_VertexID) : SV_Position
{
    float4 position = float4(positions[vertexID], 0.0, 1.0);

#ifdef INVERT_Y
    position.y = -position.y;
#endif

    return position;
}

[shader("fragment")]
float4 fragmentMain() : SV_Target
{
    return float4(1.0, 0.0, 0.0, 1.0);
}
