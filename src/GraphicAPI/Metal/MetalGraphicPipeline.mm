/*
 * ---------------------------------------------------
 * MetalGraphicPipeline.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 17:31:26
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalShader.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include <Metal/Metal.h>
#include "UtilsCPP/Macros.hpp"

namespace gfx
{

MetalGraphicPipeline::MetalGraphicPipeline(const id<MTLDevice>& mtlDevice, const GraphicPipeline::Descriptor& descriptor) { @autoreleasepool
{
    MTLVertexDescriptor* vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
    utils::uint32 i = 0;
    for (const auto& element : descriptor.vertexLayout.attributes)
    {
        vertexDescriptor.attributes[i].format = (MTLVertexFormat)toMetalVertexAttributeFormat(element.format);
        vertexDescriptor.attributes[i].offset = element.offset;
        vertexDescriptor.attributes[i].bufferIndex = 0;
        i++;
    }
    vertexDescriptor.layouts[0].stride = descriptor.vertexLayout.stride;

    MTLRenderPipelineDescriptor* renderPipelineDescriptor = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
    
    if (descriptor.vertexShader != nullptr)
        renderPipelineDescriptor.vertexFunction = dynamic_cast<MetalShader&>(*descriptor.vertexShader).mtlFunction();
    if (descriptor.fragmentShader != nullptr)
        renderPipelineDescriptor.fragmentFunction = dynamic_cast<MetalShader&>(*descriptor.fragmentShader).mtlFunction();

    renderPipelineDescriptor.vertexDescriptor = vertexDescriptor;

    renderPipelineDescriptor.colorAttachments[0].pixelFormat = (MTLPixelFormat)toMetalPixelFormat(descriptor.colorPixelFormat);
    if (descriptor.blendOperation == BlendOperation::blendingOff)
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled = NO;
    else
    {
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled = YES;
        switch (descriptor.blendOperation)
        {
        case BlendOperation::srcA_plus_1_minus_srcA:
            renderPipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
            renderPipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;

            renderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            renderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;

            renderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            renderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            break;

        case BlendOperation::one_minus_srcA_plus_srcA:
            renderPipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
            renderPipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;

            renderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            renderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

            renderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorSourceAlpha;
            renderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorSourceAlpha;
            break;

        case BlendOperation::blendingOff:
            UNREACHABLE
        }
    }

    renderPipelineDescriptor.depthAttachmentPixelFormat = (MTLPixelFormat)toMetalPixelFormat(descriptor.depthPixelFormat);

    MTLDepthStencilDescriptor* depthStencilDescriptor = [[[MTLDepthStencilDescriptor alloc] init] autorelease];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
    depthStencilDescriptor.depthWriteEnabled = YES;


    MTLAutoreleasedRenderPipelineReflection reflection = nullptr;

    m_renderPipelineState = [mtlDevice newRenderPipelineStateWithDescriptor:renderPipelineDescriptor options:MTLPipelineOptionBufferTypeInfo reflection:&reflection error:nullptr];
    if (m_renderPipelineState == nil)
        throw MTLRenderPipelineStateCreationError();

    m_depthStencilState = [mtlDevice newDepthStencilStateWithDescriptor:depthStencilDescriptor];
    if (m_renderPipelineState == nil)
        throw DepthStencilStateCreationError();


    NSArray<id<MTLBinding>>* vertexBindings = reflection.vertexBindings;
    NSArray<id<MTLBinding>>* fragmentBindings = reflection.fragmentBindings;

    for (uint32 i = 0; i < vertexBindings.count; i++)
    {
        switch (vertexBindings[i].type)
        {
        case MTLBindingTypeBuffer:
            m_vertexBufferBindingsIndices.insert([vertexBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], vertexBindings[i].index);
            break;
        case MTLBindingTypeTexture:
            m_vertexTextureBindingsIndices.insert([vertexBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], vertexBindings[i].index);
            break;
        case MTLBindingTypeSampler:
            m_vertexSamplerBindingsIndices.insert([vertexBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], vertexBindings[i].index);
            break;
        default:
            throw utils::RuntimeError("not implemented");
        }
    }

    for (uint32 i = 0; i < fragmentBindings.count; i++)
    {
        switch (fragmentBindings[i].type)
        {
        case MTLBindingTypeBuffer:
            m_fragmentBufferBindingsIndices.insert([fragmentBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], fragmentBindings[i].index);
            break;
        case MTLBindingTypeTexture:
            m_fragmentTextureBindingsIndices.insert([fragmentBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], fragmentBindings[i].index);
            break;
        case MTLBindingTypeSampler:
            m_fragmentSamplerBindingsIndices.insert([fragmentBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], fragmentBindings[i].index);
            break;
        default:
            throw utils::RuntimeError("not implemented");
        }
    }
}}

MetalGraphicPipeline::~MetalGraphicPipeline() { @autoreleasepool
{
    [m_depthStencilState release];
    [m_renderPipelineState release];
}}

}
