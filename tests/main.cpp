/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 12:58:32
 * ---------------------------------------------------
 */

#include <gtest/gtest.h>

#include "Graphics/ShaderLibrary.hpp"
#include "Logger/Logger.hpp"
#include "Graphics/Platform.hpp"
#include "UtilsCPP/String.hpp"
#include <fstream>
#ifdef IMGUI_ENABLED
    #include "imgui/imgui.h"
#endif

using namespace tlog;
using namespace gfx;

class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    void SetUp() override
    {
        Logger::init();
        Platform::init();
        ShaderLibrary::init();

        #ifdef USING_METAL
            ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
        #endif

        {
            std::ifstream f(OPENGL_SHADER_DIR"/vtx1.glsl");
            ShaderLibrary::shared().registerShader(
                "vtx1"
                #ifdef USING_METAL
                , "vtx1"
                #endif
                #ifdef USING_OPENGL
                , utils::String::contentOf(f)
                #endif
            );
        }
        {
            std::ifstream f(OPENGL_SHADER_DIR"/fra1.glsl");
            ShaderLibrary::shared().registerShader(
                "fra1"
                #ifdef USING_METAL
                , "fra1"
                #endif
                #ifdef USING_OPENGL
                , utils::String::contentOf(f)
                #endif
            );
        }
    }

    void TearDown() override
    {
        ShaderLibrary::terminated();
        Platform::terminate();
        Logger::terminate();
    }
};

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment);
    return RUN_ALL_TESTS();
}
