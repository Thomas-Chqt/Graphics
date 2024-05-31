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
#include "Graphics/Platform.hpp"

using namespace gfx;

class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    void SetUp() override
    {
        Platform::init();
        ShaderLibrary::init();

        #ifdef USING_METAL
            ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
        #endif
    }

    void TearDown() override
    {
        ShaderLibrary::terminated();
        Platform::terminate();
    }
};

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment);
    return RUN_ALL_TESTS();
}
