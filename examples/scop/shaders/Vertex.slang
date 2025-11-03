/*
 * ---------------------------------------------------
 * Vertex.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/01 16:59:20
 * ---------------------------------------------------
 */

#ifndef VERTEX_SLANG
#define VERTEX_SLANG

#include "shared_types.h"

#ifdef __cplusplus
namespace shader
{
#else

module Vertex;

#endif

SLAND_PUBLIC struct Vertex
{
    SLAND_PUBLIC float3 pos;
    SLAND_PUBLIC float2 uv;
    SLAND_PUBLIC float3 normal;
    SLAND_PUBLIC float3 tangent;
};

#ifdef __cplusplus
}
#endif

#endif
