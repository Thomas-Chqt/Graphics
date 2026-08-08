#include "shader.h"

module utils;

namespace shader
{

public float3 cubeDirection(uint face, float2 uv)
{
    switch (face)
    {
    case 0:  return normalize(float3(  1.0, -uv.y, -uv.x));
    case 1:  return normalize(float3( -1.0, -uv.y,  uv.x));
    case 2:  return normalize(float3( uv.x,   1.0,  uv.y));
    case 3:  return normalize(float3( uv.x,  -1.0, -uv.y));
    case 4:  return normalize(float3( uv.x, -uv.y,   1.0));
    default: return normalize(float3(-uv.x, -uv.y,  -1.0));
    }
}

float radicalInverseVdc(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

public float2 hammersley(uint index, uint sampleCount)
{
    return float2(float(index) / float(sampleCount), radicalInverseVdc(index));
}

public float3 reinhardToneMapping(float3 x)
{
    return x / (x + float3(1.0));
}

public float3 acesToneMapping(float3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

}
