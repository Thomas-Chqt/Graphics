/*
 * ---------------------------------------------------
 * Graphics_testCases.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/04 12:53:18
 * ---------------------------------------------------
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "MockClasses.hpp"
#include "Graphics_testFixitures.hpp"

#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/SharedPtr.hpp"

using ::testing::Return;
using ::testing::_;

namespace gfx_tests
{

#ifdef USING_GLFW

TEST_F(GfxTest, initNoError)
{
    EXPECT_CALL(mockGFLW, glfwSetErrorCallback(_))
        .Times(1);

    EXPECT_CALL(mockGFLW, glfwInit())
        .WillOnce(Return(GLFW_TRUE));

    EXPECT_NO_THROW({ gfx::Platform::init(); });


    EXPECT_CALL(mockGFLW, glfwTerminate())
        .Times(1);

    gfx::Platform::terminate();
}

/*
TEST_F(GfxTestInit, glfwInitReturnError)
{
    EXPECT_CALL(mockGFLW, glfwSetErrorCallback(_))
        .Times(1);

    EXPECT_CALL(mockGFLW, glfwInit())
        .WillOnce([&]() { return mockGFLW.sendError(123, "fake error"), -1; });

    EXPECT_THROW(
    {
        try {
            gfx::Platform::init();
        }
        catch(const std::exception& err) {
            EXPECT_THAT(err.what(), StrEq("fail to initialize GLFW. fake error"));
            throw;
        }
    }, gfx::GLFWInitError);
}
*/

#ifdef METAL_ENABLED

TEST_F(GfxTestPostInit, newMetalWindowNoError)
{
    GLFWwindow* fakeGLFWWindow = (GLFWwindow*)123;
    StrictMock<MockNSWindow> mockNSWindow;
    StrictMock<MockNSView> mockNSView;
    StrictMock<MockCAMetalLayer> mockCAMetalLayer;

    EXPECT_CALL(mockGFLW, glfwDefaultWindowHints())
        .Times(1);

    EXPECT_CALL(mockGFLW, glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API))
        .Times(1);

    EXPECT_CALL(mockGFLW, glfwCreateWindow(800, 600, "", nullptr, nullptr))
        .WillOnce(Return(fakeGLFWWindow));

    EXPECT_CALL(mockGFLW, glfwGetCocoaWindow(fakeGLFWWindow))
        .WillOnce(Return(mockNSWindow.nsWindow));

    EXPECT_CALL(mockNSWindow, getContentView())
        .WillOnce(Return(&mockNSView))
        .WillOnce(Return(&mockNSView))
        .WillOnce(Return(&mockNSView));

    EXPECT_CALL(mockNSView, setWantsLayer(1))
        .Times(1);

    EXPECT_CALL(mockNSView, setWantsLayer(1))
        .Times(1);

    EXPECT_CALL(mockCAMetalLayerStaticFuncs, layer())
        .WillOnce(Return(&mockCAMetalLayer));

    EXPECT_CALL(mockNSView, setLayer(mockCAMetalLayer.caMetalLayer))
        .Times(1);

    EXPECT_CALL(mockGFLW, glfwGetFramebufferSize(fakeGLFWWindow, _, _))
        .WillOnce([](GLFWwindow* window, int* width, int* height){
            *width = 1280;
            *height = 720;
        });

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newMetalWindow(800, 600);

    EXPECT_TRUE(window);
}

#endif

#endif

}