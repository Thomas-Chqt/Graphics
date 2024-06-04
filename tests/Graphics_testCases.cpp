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
#include <exception>

#include "GLFW/glfw3.h"
#include "Graphics/Error.hpp"
#include "IMockGLFW.hpp"

#include "Graphics/Platform.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::StrictMock;
using ::testing::StrEq;

namespace gfx_test
{

class MockGLFW : public IMockGLFW
{
public:
    MOCK_METHOD(int,          glfwInit,             (),             (override));
    MOCK_METHOD(GLFWerrorfun, glfwSetErrorCallback, (GLFWerrorfun), (override));
    MOCK_METHOD(void,         glfwTerminate,        (),             (override));

    MockGLFW()
    {
        ON_CALL(*this, glfwSetErrorCallback).WillByDefault([this](GLFWerrorfun fn) -> GLFWerrorfun { return m_errorCallback = fn; });
    }

    inline void sendError(int code, const char *desc) { m_errorCallback(code, desc); }

private:
    GLFWerrorfun m_errorCallback;
};

TEST(gfxTest, initNoError)
{
    StrictMock<MockGLFW> instance;

    EXPECT_CALL(instance, glfwSetErrorCallback(_))
        .Times(1);

    EXPECT_CALL(instance, glfwInit())
        .WillOnce(Return(GLFW_TRUE));

    EXPECT_CALL(instance, glfwTerminate())
        .Times(1);

    IMockGLFW::s_instance = &instance;

    EXPECT_NO_THROW({
        gfx::Platform::init();
        gfx::Platform::terminate();
    });
}

TEST(gfxTest, initError)
{
    StrictMock<MockGLFW> instance;

    EXPECT_CALL(instance, glfwSetErrorCallback(_))
        .Times(1);

    EXPECT_CALL(instance, glfwInit())
        .WillOnce([&]()
        {
            instance.sendError(123, "fake error");
            return -1;
        });

    IMockGLFW::s_instance = &instance;
    
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


}