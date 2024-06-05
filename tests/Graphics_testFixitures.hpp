/*
 * ---------------------------------------------------
 * Graphics_testFixitures.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 11:17:46
 * ---------------------------------------------------
 */

#ifndef GRAPHICS_TESTFIXITURES_HPP
# define GRAPHICS_TESTFIXITURES_HPP

#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "MockClasses.hpp"

#include "Graphics/Platform.hpp"

using ::testing::StrictMock;
using ::testing::Return;
using ::testing::_;

namespace gfx_tests
{

#ifdef USING_GLFW

class GfxTest : public ::testing::Test
{
protected:
    StrictMock<MockGLFW> mockGFLW;

    GfxTest()
    {
        IMockGLFW::s_instance = &mockGFLW;
    }
};

#ifdef METAL_ENABLED

class GfxTestPostInit : public GfxTest
{
protected:
    StrictMock<MockCAMetalLayerStaticFuncs> mockCAMetalLayerStaticFuncs;

    GfxTestPostInit()
    {
        IMockCAMetalLayerStaticFuncs::s_instance = &mockCAMetalLayerStaticFuncs;
        
        EXPECT_CALL(mockGFLW, glfwSetErrorCallback(_))
            .Times(1);

        EXPECT_CALL(mockGFLW, glfwInit())
            .WillOnce(Return(GLFW_TRUE));

        EXPECT_NO_THROW({ gfx::Platform::init(); });
    }

    ~GfxTestPostInit()
    {
        EXPECT_CALL(mockGFLW, glfwTerminate())
            .Times(1);

        gfx::Platform::terminate();
    }
};

#endif

#endif

}

#endif // GRAPHICS_TESTFIXITURES_HPP