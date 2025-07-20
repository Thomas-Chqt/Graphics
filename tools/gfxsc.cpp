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

    std::array<slang::TargetDesc, 2> targetDescs = {
        slang::TargetDesc{
            .format = SLANG_METAL_LIB,
            .profile = globalSession->findProfile("metal")
        },
        slang::TargetDesc{
            .format = SLANG_SPIRV,
            .profile = globalSession->findProfile("spirv")
        }
    };

    std::array<slang::CompilerOptionEntry, 3> options = {
        slang::CompilerOptionEntry{
            .name = slang::CompilerOptionName::VulkanInvertY,
            .value = slang::CompilerOptionValue { .intValue0 = 1, }
        },
        slang::CompilerOptionEntry{
            .name = slang::CompilerOptionName::EmitSpirvDirectly,
            .value = slang::CompilerOptionValue { .intValue0 = 1, }
        },
        slang::CompilerOptionEntry{
            .name = slang::CompilerOptionName::GenerateWholeProgram,
            .value = slang::CompilerOptionValue { .intValue0 = 1, }
        }
    };

    slang::SessionDesc sessionDesc = {
        .targetCount = static_cast<SlangInt>(targetDescs.size()),
        .targets = targetDescs.data(),
        .compilerOptionEntryCount = static_cast<uint32_t>(options.size()),
        .compilerOptionEntries = options.data()
    };

    Slang::ComPtr<slang::ISession> session;
    {
        SlangResult result = globalSession->createSession(sessionDesc, session.writeRef()) ;
        if (SLANG_FAILED(result))
            return std::println(stderr, "createSession: error"), 1;
    }

    std::vector<Slang::ComPtr<slang::IModule>> modules;
    std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;
    
    for (auto inputFilePath : program.get<std::vector<std::filesystem::path>>("sources"))
    {
        std::ifstream inputFileStream(inputFilePath);
        std::stringstream inputFileContent;
        inputFileContent << inputFileStream.rdbuf();
        Slang::ComPtr<slang::IModule> module;
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        module = session->loadModuleFromSourceString(
            inputFilePath.filename().replace_extension().c_str(),
            inputFilePath.c_str(),
            inputFileContent.str().c_str(),
            diagnosticsBlob.writeRef()
        );
        if (diagnosticsBlob != nullptr)
            return std::println(stderr, "{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
        if (module == nullptr)
            return std::println(stderr, "loadModule: error"), 1;
        modules.push_back(module);
        for(int i = 0; i < module->getDefinedEntryPointCount(); i++)
        {
            Slang::ComPtr<slang::IEntryPoint> entryPoint;
            SlangResult result = module->getDefinedEntryPoint(i, entryPoint.writeRef());
            if (SLANG_FAILED(result))
                return std::println(stderr, "getDefinedEntryPoint: error"), 1;
            entryPoints.push_back(entryPoint);
        }
    }

    std::vector<slang::IComponentType*> components;
    components.append_range(modules | std::views::transform([](auto& compPtr) -> slang::IComponentType* { return compPtr; }));
    components.append_range(entryPoints | std::views::transform([](auto& compPtr) -> slang::IComponentType* { return compPtr; }));

    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = session->createCompositeComponentType(components.data(), components.size(), composedProgram.writeRef(), diagnosticsBlob.writeRef());
        if (diagnosticsBlob != nullptr)
            return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
        if (SLANG_FAILED(result))
            return std::println(stderr, "createCompositeComponentType: error"), 1;
    }

    Slang::ComPtr<slang::IComponentType> linkedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = composedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
        if (diagnosticsBlob != nullptr)
            return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
        if (SLANG_FAILED(result))
            return std::println(stderr, "link: error"), 1;
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
        {
            Slang::ComPtr<slang::IBlob> targetCode;
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            if (SLANG_FAILED(linkedProgram->getTargetCode(0, targetCode.writeRef(), diagnosticsBlob.writeRef())))
                return std::println(stderr, "getTargetCode (metal): error"), 1;
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
            slang::ProgramLayout* metalLayout = linkedProgram->getLayout(0, diagnosticsBlob.writeRef());
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (metalLayout == nullptr)
                return std::println(stderr, "getLayout (metal): error"), 1;
            SlangResult result = metalLayout->toJson(targetReflection.writeRef());
            if (targetReflection == nullptr || SLANG_FAILED(result))
                return std::println(stderr, "toJson (metal): error"), 1;

            std::ofstream reflectionFile("metalRefelection.json");
            if (!reflectionFile)
                return std::println(stderr, "faild to open file"), 1;
            reflectionFile.write(reinterpret_cast<const char*>(targetReflection->getBufferPointer()), targetReflection->getBufferSize());
        }
    }

    if ((program.get<uint32_t>("--targets") & 1 << SLANG_SPIRV) != 0)
    {
        {
            Slang::ComPtr<slang::IBlob> targetCode;
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult result = linkedProgram->getTargetCode(1, targetCode.writeRef(), diagnosticsBlob.writeRef());
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (SLANG_FAILED(result))
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
            slang::ProgramLayout* spirvLayout = linkedProgram->getLayout(1, diagnosticsBlob.writeRef());
            if (diagnosticsBlob != nullptr)
                return std::println("{}", (const char*)diagnosticsBlob->getBufferPointer()), 1;
            if (spirvLayout == nullptr)
                return std::println(stderr, "getLayout (spirv): error"), 1;
            SlangResult result = spirvLayout->toJson(targetReflection.writeRef());
            if (targetReflection == nullptr || SLANG_FAILED(result))
                return std::println(stderr, "toJson (spirv): error"), 1;

            std::ofstream reflectionFile("spirvRefelection.json");
            if (!reflectionFile)
                return std::println(stderr, "faild to open file"), 1;
            reflectionFile.write(reinterpret_cast<const char*>(targetReflection->getBufferPointer()), targetReflection->getBufferSize());
        }
    }
}
