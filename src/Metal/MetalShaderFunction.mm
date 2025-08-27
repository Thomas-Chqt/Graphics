/*
 * ---------------------------------------------------
 * MetalShaderFunction.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 12:30:50
 * ---------------------------------------------------
 */

#include "Metal/MetalShaderFunction.hpp"
#include "Graphics/ShaderFunction.hpp"

namespace gfx
{

MetalShaderFunction::MetalShaderFunction(MetalShaderFunction&& other) noexcept { @autoreleasepool
{
    m_mtlFunction = [other.m_mtlFunction retain];
}}

MetalShaderFunction::MetalShaderFunction(const id<MTLLibrary>& mtlLibrary, const ext::string& name) { @autoreleasepool
{
    NSString* functionNameNSString = [[[NSString alloc] initWithCString:name.c_str() encoding:NSUTF8StringEncoding] autorelease];
    m_mtlFunction = [mtlLibrary newFunctionWithName:functionNameNSString];
    if (m_mtlFunction == nil)
        throw ext::runtime_error("failed to create the MTLFunction");
}}

MetalShaderFunction::~MetalShaderFunction() { @autoreleasepool
{
    [m_mtlFunction release];
}}

MetalShaderFunction& MetalShaderFunction::operator=(MetalShaderFunction&& other) noexcept { @autoreleasepool
{
    if (&other != this)
    {
        ShaderFunction::operator=(ext::move(other));
        m_mtlFunction = [other.m_mtlFunction retain];
    }
    return *this;
}}

}
