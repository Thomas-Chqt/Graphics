/*
 * ---------------------------------------------------
 * gfxsc.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 22:25:00
 * ---------------------------------------------------
 */

#include <cstdint>
#include <slang.h>
#include <slang-com-ptr.h>

#include <argparse/argparse.hpp>

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

// Compile shader for a specific target
// Note: The session must be kept alive as the linkedProgram has internal dependencies on it
static SlangResult compileForTarget(
    slang::IGlobalSession* globalSession,
    SlangCompileTarget targetFormat,
    const std::vector<std::filesystem::path>& sources,
    const std::vector<std::filesystem::path>& includePaths,
    slang::ISession** outSession,
    slang::IComponentType** outLinkedProgram)
{
    const char* targetName = (targetFormat == SLANG_METAL_LIB) ? "metal" : "spirv";
    const char* targetDefine = (targetFormat == SLANG_METAL_LIB) ? "__METAL__" : "__SPIRV__";

    slang::TargetDesc targetDesc = {
        .format = targetFormat,
        .profile = globalSession->findProfile(targetName)
    };

    // Preprocessor define for the target
    slang::PreprocessorMacroDesc macroDesc = {
        .name = targetDefine,
        .value = "1"
    };

    std::vector<slang::CompilerOptionEntry> options = {
        slang::CompilerOptionEntry{
            .name = slang::CompilerOptionName::GenerateWholeProgram,
            .value = slang::CompilerOptionValue { .intValue0 = 1, }
        }
    };

    // Add Vulkan-specific options for SPIRV target
    if (targetFormat == SLANG_SPIRV)
    {
        options.push_back(slang::CompilerOptionEntry{
            .name = slang::CompilerOptionName::VulkanInvertY,
            .value = slang::CompilerOptionValue { .intValue0 = 1, }
        });
        options.push_back(slang::CompilerOptionEntry{
            .name = slang::CompilerOptionName::EmitSpirvDirectly,
            .value = slang::CompilerOptionValue { .intValue0 = 1, }
        });
    }

    // Automatically add source file directories to include paths (like slangc does)
    std::vector<std::filesystem::path> allIncludePaths = includePaths;
    for (const auto& sourcePath : sources)
    {
        auto parentPath = sourcePath.parent_path();
        if (!parentPath.empty() && std::find(allIncludePaths.begin(), allIncludePaths.end(), parentPath) == allIncludePaths.end())
        {
            allIncludePaths.push_back(parentPath);
        }
    }

    // Convert include paths to C-style strings
    std::vector<std::string> includePathStrings;
    std::vector<const char*> includePathCStrs;
    for (const auto& path : allIncludePaths)
    {
        includePathStrings.push_back(path.string());
        includePathCStrs.push_back(includePathStrings.back().c_str());
    }

    slang::SessionDesc sessionDesc = {
        .targets = &targetDesc,
        .targetCount = 1,
        .searchPaths = includePathCStrs.empty() ? nullptr : includePathCStrs.data(),
        .searchPathCount = static_cast<SlangInt>(includePathCStrs.size()),
        .preprocessorMacros = &macroDesc,
        .preprocessorMacroCount = 1,
        .compilerOptionEntries = options.data(),
        .compilerOptionEntryCount = static_cast<uint32_t>(options.size())
    };

    SlangResult sessionResult = globalSession->createSession(sessionDesc, outSession);
    if (SLANG_FAILED(sessionResult))
    {
        std::println(stderr, "createSession ({}): error", targetName);
        return sessionResult;
    }

    std::vector<Slang::ComPtr<slang::IModule>> modules;
    std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;

    for (const auto& inputFilePath : sources)
    {
        std::ifstream inputFileStream(inputFilePath);
        std::stringstream inputFileContent;
        inputFileContent << inputFileStream.rdbuf();

        std::string moduleName = inputFilePath.filename().replace_extension().string();
        std::string sourcePathStr = inputFilePath.string();
        std::string sourceContent = inputFileContent.str();

        Slang::ComPtr<slang::IModule> module;
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        module = (*outSession)->loadModuleFromSourceString(
            moduleName.c_str(),
            sourcePathStr.c_str(),
            sourceContent.c_str(),
            diagnosticsBlob.writeRef()
        );
        if (diagnosticsBlob != nullptr)
        {
            std::println(stderr, "{}", (const char*)diagnosticsBlob->getBufferPointer());
            return SLANG_FAIL;
        }
        if (module == nullptr)
        {
            std::println(stderr, "loadModule ({}): error", targetName);
            return SLANG_FAIL;
        }
        modules.push_back(module);
        for(int i = 0; i < module->getDefinedEntryPointCount(); i++)
        {
            Slang::ComPtr<slang::IEntryPoint> entryPoint;
            SlangResult entryPointResult = module->getDefinedEntryPoint(i, entryPoint.writeRef());
            if (SLANG_FAILED(entryPointResult))
            {
                std::println(stderr, "getDefinedEntryPoint ({}): error", targetName);
                return entryPointResult;
            }
            entryPoints.push_back(entryPoint);
        }
    }

    std::vector<slang::IComponentType*> components;
#ifdef __cpp_lib_containers_ranges
    components.append_range(modules | std::views::transform([](auto& compPtr) -> slang::IComponentType* { return compPtr; }));
    components.append_range(entryPoints | std::views::transform([](auto& compPtr) -> slang::IComponentType* { return compPtr; }));
#else
    auto modulesRg = modules | std::views::transform([](auto& compPtr) -> slang::IComponentType* { return compPtr; });
    components.insert(components.end(), modulesRg.cbegin(), modulesRg.cend());
    auto entryPointsRg = entryPoints | std::views::transform([](auto& compPtr) -> slang::IComponentType* { return compPtr; });
    components.insert(components.end(), entryPointsRg.cbegin(), entryPointsRg.cend());
#endif

    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult composeResult = (*outSession)->createCompositeComponentType(components.data(), components.size(), composedProgram.writeRef(), diagnosticsBlob.writeRef());
        if (diagnosticsBlob != nullptr)
        {
            std::println(stderr, "{}", (const char*)diagnosticsBlob->getBufferPointer());
            return composeResult;
        }
        if (SLANG_FAILED(composeResult))
        {
            std::println(stderr, "createCompositeComponentType ({}): error", targetName);
            return composeResult;
        }
    }

    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult linkResult = composedProgram->link(outLinkedProgram, diagnosticsBlob.writeRef());
        if (diagnosticsBlob != nullptr)
        {
            std::println(stderr, "{}", (const char*)diagnosticsBlob->getBufferPointer());
            return linkResult;
        }
        if (SLANG_FAILED(linkResult))
        {
            std::println(stderr, "link ({}): error", targetName);
            return linkResult;
        }
    }

    return SLANG_OK;
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("gfxsc", "0.1");

    program.add_argument("-t", "--targets")
        .action([&](const std::string& s) -> uint32_t {
            uint32_t targets = 0;
            for (const auto& split : std::views::split(s, ','))
            {
                std::string targetStr(split.begin(), split.end());
                if (targetStr == "metal")
                    targets |= 1 << SLANG_METAL_LIB;
                else if (targetStr == "spirv")
                    targets |= 1 << SLANG_SPIRV;
                else
                    throw std::runtime_error("unkown target");
            }
            return targets;
        })
        .help("metal and/or spirv")
        .required();

    program.add_argument("-o", "--output")
        .action([&](const std::string& s) -> std::filesystem::path {
            if (s.empty() == false && s.back() == std::filesystem::path::preferred_separator)
                throw std::runtime_error("output file cannot be a directory");
            auto path = std::filesystem::path(s);
            return path;
        })
        .required();

    program.add_argument("-I")
        .action([&](const std::string& s) -> std::filesystem::path {
            auto path = std::filesystem::path(s);
            if (!std::filesystem::exists(path))
                throw std::runtime_error("include directory does not exist");
            if (!std::filesystem::is_directory(path))
                throw std::runtime_error("include path must be a directory");
            return path;
        })
        .append()
        .help("include directories for shader imports");

    program.add_argument("sources")
        .action([&](const std::string& s) -> std::filesystem::path {
            auto path = std::filesystem::path(s);
            if (!std::filesystem::exists(path))
                throw std::runtime_error("source file does not exist");
            if (std::filesystem::is_directory(path))
                throw std::runtime_error("source cannot be a directory");
            return path;
        })
        .nargs(argparse::nargs_pattern::at_least_one);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    Slang::ComPtr<slang::IGlobalSession> globalSession;
    if (SLANG_FAILED(createGlobalSession(globalSession.writeRef())))
        throw std::runtime_error("createGlobalSession: fail");

    std::vector<std::filesystem::path> includePaths;
    try {
        includePaths = program.get<std::vector<std::filesystem::path>>("-I");
    } catch (...) {
        // No include paths specified, use empty vector
    }

    std::ofstream outFile(program.get<std::filesystem::path>("--output"), std::ios::binary);
    if (!outFile)
        return std::println(stderr, "faild to open output file"), 1;
    outFile.write("GFX_SHADER_PACKAGE", 18);
    uint32_t numTargets = 0;
    if ((program.get<uint32_t>("--targets") & 1 << SLANG_METAL_LIB) != 0)
        numTargets++;
    if ((program.get<uint32_t>("--targets") & 1 << SLANG_SPIRV) != 0)
        numTargets++;
    outFile.write(reinterpret_cast<const char*>(&numTargets), sizeof(numTargets));

    if ((program.get<uint32_t>("--targets") & 1 << SLANG_METAL_LIB) != 0)
    {
        Slang::ComPtr<slang::ISession> metalSession;
        Slang::ComPtr<slang::IComponentType> metalProgram;
        SlangResult result = compileForTarget(
            globalSession,
            SLANG_METAL_LIB,
            program.get<std::vector<std::filesystem::path>>("sources"),
            includePaths,
            metalSession.writeRef(),
            metalProgram.writeRef()
        );
        if (SLANG_FAILED(result))
            return 1;

        {
            Slang::ComPtr<slang::IBlob> targetCode;
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            if (SLANG_FAILED(metalProgram->getTargetCode(0, targetCode.writeRef(), diagnosticsBlob.writeRef())))
                return std::println(stderr, "getTargetCode (metal): error: {}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;

            std::string targetName = "metal";
            uint32_t nameLen = static_cast<uint32_t>(targetName.length());
            outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outFile.write(targetName.c_str(), nameLen);

            uint32_t codeSize = static_cast<uint32_t>(targetCode->getBufferSize());
            outFile.write(reinterpret_cast<const char*>(&codeSize), sizeof(codeSize));
            outFile.write(static_cast<const char*>(targetCode->getBufferPointer()), codeSize);
        }
        {
            Slang::ComPtr<slang::IBlob> targetReflection;
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            slang::ProgramLayout* metalLayout = metalProgram->getLayout(0, diagnosticsBlob.writeRef());
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (metalLayout == nullptr)
                return std::println(stderr, "getLayout (metal): error"), 1;
            SlangResult result = metalLayout->toJson(targetReflection.writeRef());
            if (targetReflection == nullptr || SLANG_FAILED(result))
                return std::println(stderr, "toJson (metal): error"), 1;
        }
    }

    if ((program.get<uint32_t>("--targets") & 1 << SLANG_SPIRV) != 0)
    {
        Slang::ComPtr<slang::ISession> spirvSession;
        Slang::ComPtr<slang::IComponentType> spirvProgram;
        SlangResult result = compileForTarget(
            globalSession,
            SLANG_SPIRV,
            program.get<std::vector<std::filesystem::path>>("sources"),
            includePaths,
            spirvSession.writeRef(),
            spirvProgram.writeRef()
        );
        if (SLANG_FAILED(result))
            return 1;

        {
            Slang::ComPtr<slang::IBlob> targetCode;
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult codeResult = spirvProgram->getTargetCode(0, targetCode.writeRef(), diagnosticsBlob.writeRef());
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (SLANG_FAILED(codeResult))
                return std::println(stderr, "getTargetCode (spirv): error"), 1;

            std::string targetName = "spirv";
            uint32_t nameLen = static_cast<uint32_t>(targetName.length());
            outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outFile.write(targetName.c_str(), nameLen);

            uint32_t codeSize = static_cast<uint32_t>(targetCode->getBufferSize());
            outFile.write(reinterpret_cast<const char*>(&codeSize), sizeof(codeSize));
            outFile.write(static_cast<const char*>(targetCode->getBufferPointer()), codeSize);
        }
        {
            Slang::ComPtr<slang::IBlob> targetReflection;
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            slang::ProgramLayout* spirvLayout = spirvProgram->getLayout(0, diagnosticsBlob.writeRef());
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (spirvLayout == nullptr)
                return std::println(stderr, "getLayout (spirv): error"), 1;
            SlangResult result = spirvLayout->toJson(targetReflection.writeRef());
            if (targetReflection == nullptr || SLANG_FAILED(result))
                return std::println(stderr, "toJson (spirv): error"), 1;
        }
    }

    return 0;
}
