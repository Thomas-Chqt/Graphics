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

MetalShaderFunction::MetalShaderFunction(MetalShaderFunction&& other) noexcept
    : m_mtlFunction(other.m_mtlFunction)
{
}

MetalShaderFunction::MetalShaderFunction(const id<MTLLibrary>& mtlLibrary, const std::string& name) { @autoreleasepool
{
    NSString* functionNameNSString = [[NSString alloc] initWithCString:name.c_str() encoding:NSUTF8StringEncoding];
    m_mtlFunction = [mtlLibrary newFunctionWithName:functionNameNSString];
    if (m_mtlFunction == nil)
        throw std::runtime_error("failed to create the MTLFunction");
}}

MetalShaderFunction::~MetalShaderFunction() // NOLINT(modernize-use-equals-default)
{
    // force objective c destructor
}

MetalShaderFunction& MetalShaderFunction::operator=(MetalShaderFunction&& other) noexcept { @autoreleasepool
{
    if (&other != this)
    {
        ShaderFunction::operator=(std::move(other));
        m_mtlFunction = other.m_mtlFunction;
    }
    return *this;
}}

}
