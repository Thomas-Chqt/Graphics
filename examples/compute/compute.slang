#ifndef COMPUTE_SLANG
#define COMPUTE_SLANG

#include "shader.h"

#ifdef __cplusplus
namespace shader::compute
{
#else
struct SimulationResources
{
    Texture2D<float2> sourceCells;
    RWTexture2D<float2> destinationCells;
};

ParameterBlock<SimulationResources> resources;
#endif

PUSH_CONSTANT
{
    uint width;
    uint height;
    float diffusionU;
    float diffusionV;
    float feed;
    float kill;
    float timeStep;
};

#ifndef __cplusplus

[shader("compute")]
[numthreads(8, 8, 1)]
void simulate(uint3 threadId : SV_DispatchThreadID)
{
    if (threadId.x >= width || threadId.y >= height)
        return;

    const uint x = threadId.x;
    const uint y = threadId.y;
    const uint left = x == 0 ? width - 1 : x - 1;
    const uint right = x + 1 == width ? 0 : x + 1;
    const uint up = y == 0 ? height - 1 : y - 1;
    const uint down = y + 1 == height ? 0 : y + 1;

    const float2 center = resources.sourceCells[uint2(x, y)];
    float2 laplacian = -center;
    laplacian += 0.20 * resources.sourceCells[uint2(left, y)];
    laplacian += 0.20 * resources.sourceCells[uint2(right, y)];
    laplacian += 0.20 * resources.sourceCells[uint2(x, up)];
    laplacian += 0.20 * resources.sourceCells[uint2(x, down)];
    laplacian += 0.05 * resources.sourceCells[uint2(left, up)];
    laplacian += 0.05 * resources.sourceCells[uint2(right, up)];
    laplacian += 0.05 * resources.sourceCells[uint2(left, down)];
    laplacian += 0.05 * resources.sourceCells[uint2(right, down)];

    const float reaction = center.x * center.y * center.y;
    const float deltaU = diffusionU * laplacian.x - reaction + feed * (1.0 - center.x);
    const float deltaV = diffusionV * laplacian.y + reaction - (feed + kill) * center.y;

    resources.destinationCells[uint2(x, y)] = clamp(
        center + float2(deltaU, deltaV) * timeStep,
        float2(0.0, 0.0),
        float2(1.0, 1.0));
}
#endif

#ifdef __cplusplus
}
#endif

#endif
