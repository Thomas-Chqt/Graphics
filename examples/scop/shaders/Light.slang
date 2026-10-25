/*
 * ---------------------------------------------------
 * Light.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/01 17:01:32
 * ---------------------------------------------------
 */

#ifndef LIGHT_SLANG
#define LIGHT_SLANG

#include "shader.h"

#ifdef __cplusplus
namespace shader
{
#else

module Light;

#endif

SLANG_PUBLIC struct CPP_ALIGNAS(16) DirectionalLight
{
    SLANG_PUBLIC float3 position;
    FLOAT3_PADDING(0);
    SLANG_PUBLIC float3 color;
    FLOAT3_PADDING(1);
};

#if defined(__cplusplus) && defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4324)
#endif
SLANG_PUBLIC struct CPP_ALIGNAS(16) PointLight
{
    SLANG_PUBLIC float3 position;
    FLOAT3_PADDING(0);
    SLANG_PUBLIC float3 color;
    FLOAT3_PADDING(1);
    SLANG_PUBLIC float attenuation;
};
#if defined(__cplusplus) && defined(_MSC_VER)
#pragma warning(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif
