/*
 * ---------------------------------------------------
 * MetalEnums.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:17:50
 * ---------------------------------------------------
 */

#ifndef METALENUMS_HPP
#define METALENUMS_HPP

#if !defined(__OBJC__)
#error this file can only by used in objective c
#endif

#include "Graphics/Enums.hpp"

namespace gfx
{

constexpr MTLPixelFormat toMTLPixelFormat(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::RGBA8_unorm:
        return MTLPixelFormatRGBA8Unorm;
    case PixelFormat::RGBA8_sRGB:
        return MTLPixelFormatRGBA8Unorm_sRGB;
    case PixelFormat::BGRA8_unorm:
        return MTLPixelFormatBGRA8Unorm;
    case PixelFormat::BGRA8_sRGB:
        return MTLPixelFormatBGRA8Unorm_sRGB;
    case PixelFormat::RGBA16_float:
        return MTLPixelFormatRGBA16Float;
    case PixelFormat::RGBA32_float:
        return MTLPixelFormatRGBA32Float;
    case PixelFormat::RG16_float:
        return MTLPixelFormatRG16Float;
    case PixelFormat::RG32Uint:
        return MTLPixelFormatRG32Uint;
    case PixelFormat::RG32Float:
        return MTLPixelFormatRG32Float;
    case PixelFormat::Depth32Float:
        return MTLPixelFormatDepth32Float;
    }
    std::unreachable();
}

constexpr PixelFormat toPixelFormat(MTLPixelFormat pxf)
{
    switch (pxf)
    {
    case MTLPixelFormatRGBA8Unorm:
        return PixelFormat::RGBA8_unorm;
    case MTLPixelFormatRGBA8Unorm_sRGB:
        return PixelFormat::RGBA8_sRGB;
    case MTLPixelFormatBGRA8Unorm:
        return PixelFormat::BGRA8_unorm;
    case MTLPixelFormatBGRA8Unorm_sRGB:
        return PixelFormat::BGRA8_sRGB;
    case MTLPixelFormatRGBA16Float:
        return PixelFormat::RGBA16_float;
    case MTLPixelFormatRGBA32Float:
        return PixelFormat::RGBA32_float;
    case MTLPixelFormatRG16Float:
        return PixelFormat::RG16_float;
    case MTLPixelFormatRG32Uint:
        return PixelFormat::RG32Uint;
    case MTLPixelFormatRG32Float:
        return PixelFormat::RG32Float;
    case MTLPixelFormatDepth32Float:
        return PixelFormat::Depth32Float;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr MTLLoadAction toMTLLoadAction(LoadAction lac)
{
    switch (lac)
    {
    case LoadAction::load:
        return MTLLoadActionLoad;
    case LoadAction::clear:
        return MTLLoadActionClear;
    }
    std::unreachable();
}

constexpr MTLVertexFormat toMetalVertexAttributeFormat(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::float2:
        return MTLVertexFormatFloat2;
    case VertexAttributeFormat::float3:
        return MTLVertexFormatFloat3;
    case VertexAttributeFormat::float4:
        return MTLVertexFormatFloat4;
    case VertexAttributeFormat::uchar4:
        return MTLVertexFormatUChar4;
    case VertexAttributeFormat::uint:
        return MTLVertexFormatUInt;
    }
    std::unreachable();
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
    if (usages & BindingUsage::computeRead)
        mtlResourceUsage |= MTLResourceUsageRead;
    if (usages & BindingUsage::computeWrite)
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
    if (use & TextureUsage::shaderWrite)
        mtlTextureUsage |= MTLTextureUsageShaderWrite;

    return mtlTextureUsage;
}

constexpr MTLTextureType toMTLTextureType(TextureType type)
{
    switch (type)
    {
    case TextureType::texture2d:
        return MTLTextureType2D;
    case TextureType::texture2dArray:
        return MTLTextureType2DArray;
    case TextureType::textureCube:
        return MTLTextureTypeCube;
    }
    std::unreachable();
}

constexpr TextureType toTextureType(MTLTextureType type)
{
    switch (type)
    {
    case MTLTextureType2D:
        return TextureType::texture2d;
    case MTLTextureType2DArray:
        return TextureType::texture2dArray;
    case MTLTextureTypeCube:
        return TextureType::textureCube;
    default:
        throw std::runtime_error("not implemented");
    }
    std::unreachable();
}

constexpr MTLSamplerAddressMode toMTLSamplerAddressMode(SamplerAddressMode addressMode)
{
    switch (addressMode)
    {
    case SamplerAddressMode::ClampToEdge:
        return MTLSamplerAddressModeClampToEdge;
    case SamplerAddressMode::Repeat:
        return MTLSamplerAddressModeRepeat;
    case SamplerAddressMode::MirrorRepeat:
        return MTLSamplerAddressModeMirrorRepeat;
    }
    std::unreachable();
}

constexpr MTLSamplerMinMagFilter toMTLSamplerMinMagFilter(SamplerMinMagFilter filter)
{
    switch (filter)
    {
    case SamplerMinMagFilter::Nearest:
        return MTLSamplerMinMagFilterNearest;
    case SamplerMinMagFilter::Linear:
        return MTLSamplerMinMagFilterLinear;
    }
    std::unreachable();
}

constexpr MTLSamplerMipFilter toMTLSamplerMipFilter(SamplerMipFilter filter)
{
    switch (filter)
    {
    case SamplerMipFilter::NotMipmapped:
        return MTLSamplerMipFilterNotMipmapped;
    case SamplerMipFilter::Nearest:
        return MTLSamplerMipFilterNearest;
    case SamplerMipFilter::Linear:
        return MTLSamplerMipFilterLinear;
    default:
        throw std::runtime_error("not implemented");
    }
}

constexpr MTLCullMode toMTLCullMode(CullMode cullMode)
{
    switch (cullMode)
    {
    case CullMode::none:
        return MTLCullModeNone;
    case CullMode::front:
        return MTLCullModeFront;
    case CullMode::back:
        return MTLCullModeBack;
    }
    std::unreachable();
}

constexpr MTLCompareFunction toMTLCompareFunction(DepthCompareOperation operation)
{
    switch (operation)
    {
    case DepthCompareOperation::less:
        return MTLCompareFunctionLess;
    case DepthCompareOperation::lessEqual:
        return MTLCompareFunctionLessEqual;
    }
    std::unreachable();
}

}

#endif
