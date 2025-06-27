/*
 * ---------------------------------------------------
 * MetalShaderLib.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/26 12:48:14
 * ---------------------------------------------------
 */

#include "Metal/MetalShaderLib.hpp"
#include "Metal/MetalShaderFunction.hpp"
#include "Metal/MetalDevice.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <filesystem>
    #include <stdexcept>
    #include <utility>
    #include <string>
    namespace ext = std;
#endif

namespace gfx
{

MetalShaderLib::MetalShaderLib(const MetalDevice& device, const ext::filesystem::path& filepath)
    : ShaderLib(filepath) { @autoreleasepool
{
    if (m_metalBytes.empty())
        throw ext::runtime_error("No Metal shader found in the package");

    NSError* error = nil;
    dispatch_data_t data = dispatch_data_create(m_metalBytes.data(), m_metalBytes.size(), dispatch_get_main_queue(), DISPATCH_DATA_DESTRUCTOR_DEFAULT);

    m_mtlLibrary = [device.mtlDevice() newLibraryWithData:data error:&error];

    if (error != nil)
        throw ext::runtime_error([error.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding]);
}}

MetalShaderFunction& MetalShaderLib::getFunction(const ext::string& name) { @autoreleasepool
{
    auto it = m_shaderFunctions.find(name);
    if (it == m_shaderFunctions.end())
        auto res = m_shaderFunctions.emplace(ext::make_pair(name, MetalShaderFunction(m_mtlLibrary, name))).first;
    return it->second;
}}

MetalShaderLib::~MetalShaderLib() { @autoreleasepool
{ 
    m_shaderFunctions.clear();
    [m_mtlLibrary release];
}}

} // namespace gfx
