/*
 * ---------------------------------------------------
 * Enums.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 16:50:31
 * ---------------------------------------------------
 */

#ifndef GFX_ENUMS_HPP
#define GFX_ENUMS_HPP

#if defined(GFX_USE_UTILSCPP)
#else
#endif

namespace gfx
{

enum class PixelFormat
{
    BGRA8Unorm,
    BGRA8Unorm_sRGB
};

enum class PresentMode
{
    fifo,
    mailbox
};

enum class LoadAction
{
    load,
    clear
};

enum class BlendOperation
{
    blendingOff,
    srcA_plus_1_minus_srcA,
    one_minus_srcA_plus_srcA
};

enum class VertexAttributeFormat
{
    float2,
    float3
};

enum class BufferUsage
{
    vertexBuffer
};

} // namespace gfx

#endif // GFX_ENUMS_HPP
