/*
 * ---------------------------------------------------
 * MockClasses.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 19:06:44
 * ---------------------------------------------------
 */

#ifndef MOCKCLASSES_HPP
# define MOCKCLASSES_HPP

#include <gtest/gtest.h>
#include "gmock/gmock.h"

#ifdef USING_GLFW
    #include "GLFW/IMockGLFW.hpp"
#endif

#ifdef USING_METAL
    #include "AppKit/IMockNSWindow.hpp"
    #include "QuartzCore/IMockCAMetalLayer.hpp"
#endif

namespace gfx_tests
{

#ifdef USING_GLFW

class MockGLFW : public IMockGLFW
{
public:
    MOCK_METHOD(int,          glfwInit,               (),                                                 (override));
    MOCK_METHOD(GLFWerrorfun, glfwSetErrorCallback,   (GLFWerrorfun),                                     (override));
    MOCK_METHOD(void,         glfwTerminate,          (),                                                 (override));
    MOCK_METHOD(void,         glfwDefaultWindowHints, (),                                                 (override));
    MOCK_METHOD(void,         glfwWindowHint,         (int, int),                                         (override));
    MOCK_METHOD(GLFWwindow*,  glfwCreateWindow,       (int, int, const char*, GLFWmonitor*, GLFWwindow*), (override));
    MOCK_METHOD(void,         glfwGetFramebufferSize, (GLFWwindow*, int*, int*),                          (override));
    
    #ifdef METAL_ENABLED
        MOCK_METHOD(id, glfwGetCocoaWindow, (GLFWwindow*), (override));
    #endif

    MockGLFW()
    {
        ON_CALL(*this, glfwSetErrorCallback).WillByDefault([this](GLFWerrorfun fn) -> GLFWerrorfun { return m_errorCallback = fn; });
    }

    inline void sendError(int code, const char *desc) { m_errorCallback(code, desc); }

private:
    GLFWerrorfun m_errorCallback;
};

#endif

#ifdef USING_METAL

class MockNSWindow : public IMockNSWindow
{
public:
    MOCK_METHOD(IMockNSView*, getContentView, (), (override));
};

class MockNSView : public IMockNSView
{
public:
    MOCK_METHOD(void,               setWantsLayer, (int),   (override));
    MOCK_METHOD(IMockCAMetalLayer*, getLayer,      (),      (override));
    MOCK_METHOD(void,               setLayer,      (void*), (override));
};

class MockCAMetalLayer : public IMockCAMetalLayer
{
public:
    MOCK_METHOD(CGSize, getDrawableSize, (), (override));
};

class MockCAMetalLayerStaticFuncs : public IMockCAMetalLayerStaticFuncs
{
public:
    MOCK_METHOD(IMockCAMetalLayer*, layer, (), (override));
};

#endif

}

#endif // MOCKCLASSES_HPP