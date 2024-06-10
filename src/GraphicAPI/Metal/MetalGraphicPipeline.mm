/*
 * ---------------------------------------------------
 * MetalGraphicPipeline.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 17:31:26
 * ---------------------------------------------------
 */

#include "Graphics/Error.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/ShaderLibrary.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"

namespace gfx
{

const id<MTLRenderPipelineState>& MetalGraphicPipeline::makeRenderPipelineState(id<MTLDevice> mtlDevice, MTLRenderPipelineDescriptor* decriptor) { @autoreleasepool
{
    if (m_renderPipelineState)
        [m_renderPipelineState release];
    m_fragmentUniformsIndices.clear();
    m_vertexUniformsIndices.clear();
    
    decriptor.vertexFunction = m_vertexFunction;
    decriptor.fragmentFunction = m_fragmentFunction;

    NSError* error;
    MTLAutoreleasedRenderPipelineReflection reflection;

    m_renderPipelineState = [mtlDevice newRenderPipelineStateWithDescriptor:decriptor options:MTLPipelineOptionBufferTypeInfo reflection:&reflection error:&error];
    if (!m_renderPipelineState)
        throw MTLRenderPipelineStateCreationError();

    auto vertexBindings = reflection.vertexArguments;
    auto fragmentBindings = reflection.fragmentArguments;

    for (uint32 i = 0; i < vertexBindings.count; i++)
        m_vertexUniformsIndices.insert([vertexBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], vertexBindings[i].index);

    for (uint32 i = 0; i < fragmentBindings.count; i++)
        m_fragmentUniformsIndices.insert([fragmentBindings[i].name cStringUsingEncoding:NSUTF8StringEncoding], fragmentBindings[i].index);

    return m_renderPipelineState;
}}

MetalGraphicPipeline::~MetalGraphicPipeline() { @autoreleasepool
{
    if (m_renderPipelineState)
        [m_renderPipelineState release];
    [m_mtlLibrary release];
    [m_fragmentFunction release];
    [m_vertexFunction release];
}}

MetalGraphicPipeline::MetalGraphicPipeline(id<MTLDevice> mtlDevice, const utils::String& vertexShaderName, const utils::String& fragmentShaderName) { @autoreleasepool
{
    NSError *error;
    NSString* mtlShaderLibPath = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderLibPath() encoding:NSUTF8StringEncoding] autorelease];
    m_mtlLibrary = [mtlDevice newLibraryWithURL:[NSURL URLWithString: mtlShaderLibPath] error:&error];
    if (!m_mtlLibrary)
        throw MTLLibraryCreationError();

    NSString* vertexShaderFuncName = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderFuncName(vertexShaderName) encoding:NSUTF8StringEncoding] autorelease];
    m_vertexFunction = [m_mtlLibrary newFunctionWithName:vertexShaderFuncName];
    if (!m_vertexFunction)
        throw MTLFunctionCreationError();

    NSString* fragmentShaderFuncName = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderFuncName(fragmentShaderName) encoding:NSUTF8StringEncoding] autorelease];
    m_fragmentFunction = [m_mtlLibrary newFunctionWithName:fragmentShaderFuncName];
    if (!m_fragmentFunction)
        throw MTLFunctionCreationError();
}}

}
