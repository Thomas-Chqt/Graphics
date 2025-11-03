/*
 * ---------------------------------------------------
 * shared_types.h
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/01 16:57:13
 * ---------------------------------------------------
 */

#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#ifdef __cplusplus

# include <glm/glm.hpp>

using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;

# define SLAND_PUBLIC
# define CPP_ALIGNAS(n) alignas(n) // NOLINT(cppcoreguidelines-macro-usage)
# define FLOAT3_PADDING(n) float _padding##n

#else

# define SLAND_PUBLIC public
# define CPP_ALIGNAS(n)

# ifdef __SPIRV__
#  define FLOAT3_PADDING(n) float _padding##n
# else
#  define FLOAT3_PADDING(n)
# endif

#endif

#endif
