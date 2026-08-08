#ifndef SHADER_H
#define SHADER_H

#ifdef __cplusplus
    #if !defined(SCOP_MANDATORY)
        #include <glm/glm.hpp>
    #else
        #include "math/math.hpp"
        #ifndef SCOP_MATH_GLM_ALIAS_DEFINED
            #define SCOP_MATH_GLM_ALIAS_DEFINED
namespace glm = scop::math;
        #endif
    #endif
#endif

namespace shader
{

#ifdef __cplusplus
    using float2 = glm::vec2;
    using float3 = glm::vec3;
    using float4 = glm::vec4;
    using float4x4 = glm::mat4;
    #define SLANG_PUBLIC
    #define SLANG_MODULE_DEF(name)
    #define SLANG_MODULE_IMP(name)
    #define CPP_ALIGNAS(n) alignas(n)
    #define CBUFFER_BEGIN(name)
    #define CBUFFER_END
    #ifndef FLOAT3_PADDING
        #define FLOAT3_PADDING(n) float _padding3##n
    #endif
    #define FLOAT1_PADDING(n) float _padding1a##n, _padding1b##n, _padding1c##n
    #define UINT32_PADDING(n) uint32_t _paddinguinta##n, _paddinguintb##n, _paddinguintc##n
    #define PUSH_CONSTANT struct PushConstants
#else
    #define SLANG_PUBLIC public
    #define SLANG_MODULE_DEF(name) module name
    #define SLANG_MODULE_IMP(name) import name
    #define CPP_ALIGNAS(n)
    #define alignas(n)
    #define CBUFFER_BEGIN(name) cbuffer name {
    #define CBUFFER_END };
    #ifdef __SPIRV__
        #define FLOAT3_PADDING(n) float _padding3##n
        #define FLOAT1_PADDING(n) float _padding1a##n, _padding1b##n, _padding1c##n
        #define UINT32_PADDING(n) uint32_t _paddinguinta##n, _paddinguintb##n, _paddinguintc##n
    #else
        #define FLOAT3_PADDING(n)
        #define FLOAT1_PADDING(n)
        #define UINT32_PADDING(n)
    #endif
    #ifdef __METAL__
        #define PUSH_CONSTANT [[vk::push_constant]] cbuffer PushConstant : register(b6)
        #define INLINE_PUSH_CONSTANT(T, name) uniform ConstantBuffer<T> name : register(b6)
    #else
        #define PUSH_CONSTANT [[vk::push_constant]] cbuffer PushConstant
        #define INLINE_PUSH_CONSTANT(T, name) uniform T name
    #endif
#endif

}
#endif
