/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 12:58:32
 * ---------------------------------------------------
 */

#include <gtest/gtest.h>

#include "Logger/Logger.hpp"
#include "Graphics/Platform.hpp"

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
    }

    void TearDown() override
    {
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
