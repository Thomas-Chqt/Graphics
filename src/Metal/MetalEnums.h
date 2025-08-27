/*
 * ---------------------------------------------------
 * MetalEnums.h
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:17:50
 * ---------------------------------------------------
 */

#ifndef __OBJC__
#error "this file can only by used in objective c"
#endif

#include "Graphics/Enums.hpp"

namespace gfx
{

constexpr MTLPixelFormat toMTLPixelFormat(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::RGBA8Unorm:
        return MTLPixelFormatRGBA8Unorm;
    case PixelFormat::BGRA8Unorm:
        return MTLPixelFormatBGRA8Unorm;
    case PixelFormat::BGRA8Unorm_sRGB:
        return MTLPixelFormatBGRA8Unorm_sRGB;
    case PixelFormat::Depth32Float:
        return MTLPixelFormatDepth32Float;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr PixelFormat toPixelFormat(MTLPixelFormat pxf)
{
    switch (pxf)
    {
    case MTLPixelFormatRGBA8Unorm:
        return PixelFormat::RGBA8Unorm;
    case MTLPixelFormatBGRA8Unorm:
        return PixelFormat::BGRA8Unorm;
    case MTLPixelFormatBGRA8Unorm_sRGB:
        return PixelFormat::BGRA8Unorm_sRGB;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr MTLLoadAction toMTLLoadAction(LoadAction lac)
{
    switch (lac)
    {
    case LoadAction::load:
        return MTLLoadActionLoad;
    case LoadAction::clear:
        return MTLLoadActionClear;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr MTLVertexFormat toMetalVertexAttributeFormat(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::float2:
        return MTLVertexFormatFloat2;
    case VertexAttributeFormat::float3:
        return MTLVertexFormatFloat3;
    default:
        throw ext::runtime_error("not implemented");
    }
}

constexpr MTLResourceUsage toMTLResourceUsage(BindingUsages usages)
{
    MTLResourceUsage mtlResourceUsage = 0;

    if (usages & BindingUsage::vertexRead)
        mtlResourceUsage |= MTLResourceUsageRead;
    if (usages & BindingUsage::vertexWrite)
        mtlResourceUsage |= MTLResourceUsageWrite;
    if (usages & BindingUsage::fragmentRead)
        mtlResourceUsage |= MTLResourceUsageRead;
    if (usages & BindingUsage::fragmentWrite)
        mtlResourceUsage |= MTLResourceUsageWrite;
    
    return mtlResourceUsage;
}

constexpr MTLRenderStages toMTLRenderStages(BindingUsages usages)
{
    MTLRenderStages mtlRenderStages = 0;

    if (usages & BindingUsage::vertexRead)
        mtlRenderStages |= MTLRenderStageVertex;
    if (usages & BindingUsage::vertexWrite)
        mtlRenderStages |= MTLRenderStageVertex;
    if (usages & BindingUsage::fragmentRead)
        mtlRenderStages |= MTLRenderStageFragment;
    if (usages & BindingUsage::fragmentWrite)
        mtlRenderStages |= MTLRenderStageFragment;
    
    return mtlRenderStages;
}

constexpr MTLTextureUsage toMTLTextureUsage(TextureUsages use)
{
    MTLTextureUsage mtlTextureUsage = 0;

    if (use & TextureUsage::shaderRead)
        mtlTextureUsage |= MTLTextureUsageShaderRead;
    if (use & TextureUsage::colorAttachment)
        mtlTextureUsage |= MTLTextureUsageRenderTarget;
    if (use & TextureUsage::depthStencilAttachment)
        mtlTextureUsage |= MTLTextureUsageRenderTarget;

    return mtlTextureUsage;
}

constexpr MTLTextureType toMTLTextureType(TextureType type)
{
    switch (type)
    {
    case TextureType::texture2d:
        return MTLTextureType2D;
    default:
        throw ext::runtime_error("not implemented");
    }
}

}
