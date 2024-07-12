/*
 * ---------------------------------------------------
 * MetalShader.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 14:14:55
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalShader.hpp"
#include "Graphics/Error.hpp"
#include "Graphics/Shader.hpp"

namespace gfx
{

MetalShader::MetalShader(const id<MTLDevice>& mtlDevice, const Shader::Descriptor& descriptor)
    : m_shaderType(descriptor.type) { @autoreleasepool
{
    NSString* mtlShaderLibPath = [[[NSString alloc] initWithCString:descriptor.mtlShaderLibPath encoding:NSUTF8StringEncoding] autorelease];
    id<MTLLibrary> mtlLibrary = [[mtlDevice newLibraryWithURL:[NSURL URLWithString: mtlShaderLibPath] error:nullptr] autorelease];
    if (mtlLibrary == nil)
        throw MTLLibraryCreationError();

    NSString* functionNameNSString = [[[NSString alloc] initWithCString:descriptor.mtlFunction encoding:NSUTF8StringEncoding] autorelease];
    m_mtlFunction = [mtlLibrary newFunctionWithName:functionNameNSString];
    if (m_mtlFunction == nil)
        throw MTLFunctionCreationError();
}}

MetalShader::~MetalShader() { @autoreleasepool
{
    [m_mtlFunction release];
}}

}