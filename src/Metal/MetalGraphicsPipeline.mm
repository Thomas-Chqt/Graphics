/*
 * ---------------------------------------------------
 * MetalGraphicsPipeline.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 06:48:56
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/GraphicsPipeline.hpp"

#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalShaderFunction.hpp"
#include "Metal/MetalEnums.hpp"
    
#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <utility>
    #include <cstdint>
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

MetalGraphicsPipeline::MetalGraphicsPipeline(const MetalDevice& device, const GraphicsPipeline::Descriptor& desc) { @autoreleasepool
{
    MTLRenderPipelineDescriptor* renderPipelineDescriptor = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
    
    if (desc.vertexShader != nullptr)
        renderPipelineDescriptor.vertexFunction = dynamic_cast<MetalShaderFunction&>(*desc.vertexShader).mtlFunction();
    if (desc.fragmentShader != nullptr)
        renderPipelineDescriptor.fragmentFunction = dynamic_cast<MetalShaderFunction&>(*desc.fragmentShader).mtlFunction();

    for (uint32_t i = 0; const PixelFormat& pxFmt : desc.colorAttachmentPxFormats)
    {
        renderPipelineDescriptor.colorAttachments[i].pixelFormat = (MTLPixelFormat)toMTLPixelFormat(pxFmt);
        if (desc.blendOperation == BlendOperation::blendingOff)
            renderPipelineDescriptor.colorAttachments[i].blendingEnabled = NO;
        else
        {
            renderPipelineDescriptor.colorAttachments[i].blendingEnabled = YES;
            switch (desc.blendOperation)
            {
            case BlendOperation::srcA_plus_1_minus_srcA:
                renderPipelineDescriptor.colorAttachments[i].rgbBlendOperation = MTLBlendOperationAdd;
                renderPipelineDescriptor.colorAttachments[i].alphaBlendOperation = MTLBlendOperationAdd;

                renderPipelineDescriptor.colorAttachments[i].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                renderPipelineDescriptor.colorAttachments[i].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;

                renderPipelineDescriptor.colorAttachments[i].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                renderPipelineDescriptor.colorAttachments[i].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                break;

            case BlendOperation::one_minus_srcA_plus_srcA:
                renderPipelineDescriptor.colorAttachments[i].rgbBlendOperation = MTLBlendOperationAdd;
                renderPipelineDescriptor.colorAttachments[i].alphaBlendOperation = MTLBlendOperationAdd;

                renderPipelineDescriptor.colorAttachments[i].sourceRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                renderPipelineDescriptor.colorAttachments[i].sourceAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

                renderPipelineDescriptor.colorAttachments[i].destinationRGBBlendFactor = MTLBlendFactorSourceAlpha;
                renderPipelineDescriptor.colorAttachments[i].destinationAlphaBlendFactor = MTLBlendFactorSourceAlpha;
                break;

            case BlendOperation::blendingOff:
                ext::unreachable();
            }
        }
    }

    m_renderPipelineState = [device.mtlDevice() newRenderPipelineStateWithDescriptor:renderPipelineDescriptor error:nil];
    if (m_renderPipelineState == nil)
        throw ext::runtime_error("failed to create the RenderPipelineState");
    
    if (auto& depthPxFmt = desc.depthAttachmentPxFormat)
    {
        renderPipelineDescriptor.depthAttachmentPixelFormat = (MTLPixelFormat)toMTLPixelFormat(*depthPxFmt);

        MTLDepthStencilDescriptor* depthStencilDescriptor = [[[MTLDepthStencilDescriptor alloc] init] autorelease];
        depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
        depthStencilDescriptor.depthWriteEnabled = YES;

        m_depthStencilState = [device.mtlDevice() newDepthStencilStateWithDescriptor:depthStencilDescriptor];
        if (m_renderPipelineState == nil)
            throw ext::runtime_error("failed to create DepthStencilState");
    }
}}

MetalGraphicsPipeline::~MetalGraphicsPipeline() { @autoreleasepool
{
    if (m_depthStencilState)
        [m_depthStencilState release];
    [m_renderPipelineState release];
}}

}
