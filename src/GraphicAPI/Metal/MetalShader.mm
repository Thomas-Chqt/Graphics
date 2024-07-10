/*
 * ---------------------------------------------------
 * MetalShader.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 14:14:55
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalShader.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"

namespace gfx
{

MetalShader::MetalShader(const id<MTLDevice>& mtlDevice, const MetalShaderDescriptor& descriptor)
    : m_shaderType(descriptor.type) { @autoreleasepool
{
    NSString* mtlShaderLibPath = [[[NSString alloc] initWithCString:descriptor.mtlShaderLibPath encoding:NSUTF8StringEncoding] autorelease];
    id<MTLLibrary> mtlLibrary = [[mtlDevice newLibraryWithURL:[NSURL URLWithString: mtlShaderLibPath] error:nullptr] autorelease];
    if (!mtlLibrary)
        throw MTLLibraryCreationError();

    NSString* functionNameNSString = [[[NSString alloc] initWithCString:descriptor.mtlFunction encoding:NSUTF8StringEncoding] autorelease];
    m_mtlFunction = [mtlLibrary newFunctionWithName:functionNameNSString];
    if (!m_mtlFunction)
        throw MTLFunctionCreationError();
}}

MetalShader::~MetalShader() { @autoreleasepool
{
    [m_mtlFunction release];
}}

}