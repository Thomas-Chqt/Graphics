/*
 * ---------------------------------------------------
 * MetalGraphicPipeline.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 17:31:26
 * ---------------------------------------------------
 */



#include "Graphics/ShaderLibrary.hpp"
#include <cassert>
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"

namespace gfx
{

MetalGraphicPipeline::~MetalGraphicPipeline() { @autoreleasepool
{
    [m_renderPipelineState release];
}}

MetalGraphicPipeline::MetalGraphicPipeline(id<MTLDevice> mtlDevice, id<MTLLibrary> mtlLibrary, CAMetalLayer* metalLayer, const utils::String& vertexShaderName, const utils::String& fragmentShaderName) { @autoreleasepool
{
    NSString* vertexShaderFuncName = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderFuncName(vertexShaderName) encoding:NSUTF8StringEncoding] autorelease];
    id<MTLFunction> vertexFunction = [[mtlLibrary newFunctionWithName:vertexShaderFuncName] autorelease];
    assert(vertexFunction);

    NSString* fragmentShaderFuncName = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderFuncName(fragmentShaderName) encoding:NSUTF8StringEncoding] autorelease];
    id<MTLFunction> fragmentFunction = [[mtlLibrary newFunctionWithName:fragmentShaderFuncName] autorelease];
    assert(fragmentFunction);

    MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
    assert(pipelineStateDescriptor);

    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;

    pipelineStateDescriptor.colorAttachments[0].pixelFormat = metalLayer.pixelFormat;

    NSError* error;
    MTLAutoreleasedRenderPipelineReflection reflection;

    m_renderPipelineState = [mtlDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor options:MTLPipelineOptionBufferTypeInfo reflection:&reflection error:&error];
    assert(m_renderPipelineState);

    auto vertexBindings = reflection.vertexBindings;
    auto fragmentBindings = reflection.fragmentBindings;

    for (uint32 i = 0; i < vertexBindings.count; i++)
        m_vertexUniformsIndices.insert([vertexBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], vertexBindings[i].index);

    for (uint32 i = 0; i < fragmentBindings.count; i++)
        m_fragmentUniformsIndices.insert([fragmentBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], fragmentBindings[i].index);
}}

}
