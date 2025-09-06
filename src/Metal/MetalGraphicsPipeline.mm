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

#import "Metal/MetalEnums.h"
    
namespace gfx
{

MetalGraphicsPipeline::MetalGraphicsPipeline(MetalGraphicsPipeline&& other) noexcept
    : m_renderPipelineState(ext::exchange(other.m_renderPipelineState, nil)),
      m_depthStencilState(ext::exchange(other.m_depthStencilState, nil))
{
}

MetalGraphicsPipeline::MetalGraphicsPipeline(const MetalDevice& device, const GraphicsPipeline::Descriptor& desc) { @autoreleasepool
{
    assert(desc.vertexShader != nullptr);
    assert(desc.fragmentShader != nullptr);

    MTLRenderPipelineDescriptor* renderPipelineDescriptor = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
    MTLDepthStencilDescriptor* depthStencilDescriptor = nil;
    
    if (auto& vertexLayout = desc.vertexLayout)
    {
        MTLVertexDescriptor* vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
        vertexDescriptor.layouts[5].stride = vertexLayout->stride;
        for (uint32_t i = 0; const auto& element : vertexLayout->attributes)
        {
            vertexDescriptor.attributes[i].format = (MTLVertexFormat)toMetalVertexAttributeFormat(element.format);
            vertexDescriptor.attributes[i].offset = element.offset;
            vertexDescriptor.attributes[i].bufferIndex = 5;
            i++;
        }
        renderPipelineDescriptor.vertexDescriptor = vertexDescriptor;
    }

    renderPipelineDescriptor.vertexFunction = dynamic_cast<MetalShaderFunction&>(*desc.vertexShader).mtlFunction();
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

    if (auto& depthPxFmt = desc.depthAttachmentPxFormat)
    {
        renderPipelineDescriptor.depthAttachmentPixelFormat = (MTLPixelFormat)toMTLPixelFormat(*depthPxFmt);
        depthStencilDescriptor = [[[MTLDepthStencilDescriptor alloc] init] autorelease];
        depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
        depthStencilDescriptor.depthWriteEnabled = YES;
    }

    m_renderPipelineState = [device.mtlDevice() newRenderPipelineStateWithDescriptor:renderPipelineDescriptor error:nil];
    if (m_renderPipelineState == nil)
        throw ext::runtime_error("failed to create the RenderPipelineState");
    
    if (depthStencilDescriptor != nil)
    {
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

MetalGraphicsPipeline& MetalGraphicsPipeline::operator=(MetalGraphicsPipeline&& other) noexcept
{
    if (this != &other)
    {
        if (m_depthStencilState)
            [m_depthStencilState release];
        [m_renderPipelineState release];
        m_renderPipelineState = ext::exchange(other.m_renderPipelineState, nil);
        m_depthStencilState = ext::exchange(other.m_depthStencilState, nil);
    }
    return *this;
}

}
