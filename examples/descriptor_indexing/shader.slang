/*
 * ---------------------------------------------------
 * shader.h
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * ---------------------------------------------------
 */

#ifndef SHADER_SLANG
#define SHADER_SLANG

#include "shader.h"

#define MAX_TEXTURES 4096

struct TextureArray
{
    SamplerState sampler;
    Texture2D<float4> textures[MAX_TEXTURES];
};
ParameterBlock<TextureArray> textureArray;

PUSH_CONSTANT
{
    float2 pos;
    float rotation;
    float size;
    uint32_t textureIdx;
}

struct Vertex
{
    float2 pos;
};

struct VSOutput
{
    float4 clipPos : SV_Position;
    float2 uv;
};

[shader("vertex")]
VSOutput vertexMain(Vertex input)
{
    VSOutput output;
    const float c = cos(rotation);
    const float s = sin(rotation);
    const float2 scaledPos = input.pos * size;
    const float2 rotatedPos = float2(
        scaledPos.x * c - scaledPos.y * s,
        scaledPos.x * s + scaledPos.y * c
    );

    output.clipPos = float4(rotatedPos + pos, 0.0, 1.0);
    output.uv = input.pos + 0.5;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_TARGET
{
    return textureArray.textures[textureIdx].Sample(textureArray.sampler, input.uv);
}

#endif
