/*
 * ---------------------------------------------------
 * shader.h
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * ---------------------------------------------------
 *
 * file included by all slang files
 *
 */

#ifndef SHADER_H
#define SHADER_H

#ifdef __cplusplus
    #include <glm/glm.hpp>
    using float2 = glm::vec2;
    using float3 = glm::vec3;
    using float4 = glm::vec4;
    using float4x4 = glm::mat4;
    #define SLANG_PUBLIC
    #define SLANG_MODULE_DEF(name)
    #define SLANG_MODULE_IMP(name)
    #define CBUFFER_BEGIN(name)
    #define CBUFFER_END
    #define FLOAT3_PADDING(n) float _padding3##n
    #define FLOAT1_PADDING(n) float _padding1a##n, _padding1b##n, _padding1c##n
#else
    #define SLANG_PUBLIC public
    #define SLANG_MODULE_DEF(name) module name
    #define SLANG_MODULE_IMP(name) import name
    #define alignas(n)
    #define CBUFFER_BEGIN(name) cbuffer name {
    #define CBUFFER_END };
    #ifdef __SPIRV__
        #define FLOAT3_PADDING(n) float _padding3##n
        #define FLOAT1_PADDING(n) float _padding1a##n, _padding1b##n, _padding1c##n
    #else
        #define FLOAT3_PADDING(n)
        #define FLOAT1_PADDING(n)
    #endif
    #ifdef __METAL__
        #define PUSH_CONSTANT [[vk::push_constant]] cbuffer PushConstant : register(b6)
    #else
        #define PUSH_CONSTANT [[vk::push_constant]] cbuffer PushConstant
    #endif
#endif

#endif
