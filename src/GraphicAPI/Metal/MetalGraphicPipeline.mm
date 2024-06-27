/*
 * ---------------------------------------------------
 * MetalGraphicPipeline.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 17:31:26
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include <Metal/Metal.h>
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"

namespace gfx
{

MetalGraphicPipeline::MetalGraphicPipeline(id<MTLDevice> mtlDevice, id<MTLLibrary> mtlLibrary, const GraphicPipeline::Descriptor& descriptor) { @autoreleasepool
{
    NSString* vertexShaderFuncName = [[[NSString alloc] initWithCString:descriptor.metalVSFunction encoding:NSUTF8StringEncoding] autorelease];
    id<MTLFunction> vertexFunction = [[mtlLibrary newFunctionWithName:vertexShaderFuncName] autorelease];
    if (!vertexFunction)
        throw MTLFunctionCreationError();

    NSString* fragmentShaderFuncName = [[[NSString alloc] initWithCString:descriptor.metalFSFunction encoding:NSUTF8StringEncoding] autorelease];
    id<MTLFunction> fragmentFunction = [[mtlLibrary newFunctionWithName:fragmentShaderFuncName] autorelease];
    if (!fragmentFunction)
        throw MTLFunctionCreationError();

    MTLRenderPipelineDescriptor* renderPipelineDescriptor = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
    
    renderPipelineDescriptor.vertexFunction = vertexFunction;
    renderPipelineDescriptor.fragmentFunction = fragmentFunction;
    renderPipelineDescriptor.colorAttachments[0].pixelFormat = (MTLPixelFormat)toMetalPixelFormat(descriptor.colorPixelFormat);
    if (descriptor.blendOperation != BlendOperation::blendingOff)
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
            #if defined(_MSC_VER) && !defined(__clang__) // MSVC
                __assume(false);
            #else // GCC, Clang
                __builtin_unreachable();
            #endif
        }
    }
    else
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled = NO;

    renderPipelineDescriptor.depthAttachmentPixelFormat = (MTLPixelFormat)toMetalPixelFormat(descriptor.depthPixelFormat);

    NSError* error;
    MTLAutoreleasedRenderPipelineReflection reflection;

    m_renderPipelineState = [mtlDevice newRenderPipelineStateWithDescriptor:renderPipelineDescriptor options:MTLPipelineOptionBufferTypeInfo reflection:&reflection error:&error];
    if (!m_renderPipelineState)
        throw MTLRenderPipelineStateCreationError();

    auto vertexBindings = reflection.vertexBindings;
    auto fragmentBindings = reflection.fragmentBindings;

    for (uint32 i = 0; i < vertexBindings.count; i++)
        m_vertexUniformsIndices.insert([vertexBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], vertexBindings[i].index);

    for (uint32 i = 0; i < fragmentBindings.count; i++)
        m_fragmentUniformsIndices.insert([fragmentBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], fragmentBindings[i].index);

    MTLDepthStencilDescriptor* depthStencilDescriptor = [[[MTLDepthStencilDescriptor alloc] init] autorelease];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
    depthStencilDescriptor.depthWriteEnabled = YES;
    m_depthStencilState = [mtlDevice newDepthStencilStateWithDescriptor:depthStencilDescriptor];
}}

MetalGraphicPipeline::~MetalGraphicPipeline() { @autoreleasepool
{
    [m_renderPipelineState release];
}}

}
