/*
 * ---------------------------------------------------
 * pch.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/26 06:44:10
 * ---------------------------------------------------
 */

#ifndef GRAPHICS_PCH_HPP
#define GRAPHICS_PCH_HPP

#include <cstdint>    // IWYU pragma: keep
#include <stdexcept>  // IWYU pragma: keep
#include <cstring>    // IWYU pragma: keep
#include <utility>    // IWYU pragma: keep
#include <cassert>    // IWYU pragma: keep
#include <cstddef>    // IWYU pragma: keep
#include <memory>     // IWYU pragma: keep
#include <set>        // IWYU pragma: keep
#include <algorithm>  // IWYU pragma: keep
#include <ranges>     // IWYU pragma: keep
#include <deque>      // IWYU pragma: keep
#include <array>      // IWYU pragma: keep
#include <vector>     // IWYU pragma: keep
#include <iterator>   // IWYU pragma: keep
#include <map>        // IWYU pragma: keep
#include <string>     // IWYU pragma: keep
#include <filesystem> // IWYU pragma: keep
#include <fstream>    // IWYU pragma: keep
#include <print>      // IWYU pragma: keep
#include <ctime>      // IWYU pragma: keep
#include <mutex>      // IWYU pragma: keep

#if defined(GFX_BUILD_METAL)
#if defined(__OBJC__)

#import <TargetConditionals.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <AppKit/NSWindow.h>

#else // __OBJC__

template<typename T> using id = T*;
#define nil nullptr
class MTLBuffer;
class MTLCommandQueue;
class MTLCommandBuffer;
class MTLCommandEncoder;
class MTLDevice;
class CAMetalDrawable;
class MTLRenderPipelineState;
class MTLDepthStencilState;
class NSAutoreleasePool;
class MTLLibrary;
class MTLFunction;
class CAMetalLayer;
class MTLTexture;
class MTLSamplerState;

#endif // __OBJC__
#endif // GFX_BUILD_METAL

#if defined (GFX_BUILD_VULKAN)

#include <vulkan/vulkan.hpp>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wall"
#  pragma clang diagnostic ignored "-Wextra"
#  pragma clang diagnostic ignored "-Wpedantic"
#  pragma clang diagnostic ignored "-Wnullability-completeness"
#elif defined(_MSC_VER)
#  pragma warning(push, 0)
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wall"
#  pragma GCC diagnostic ignored "-Wextra"
#  pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include <vk_mem_alloc.h>

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif
#endif // GFX_BUILD_VULKAN

#if defined(GFX_GLFW_ENABLED) || defined(GFX_IMGUI_ENABLED)

#include <dlLoad/dlLoad.h>

#endif

#if defined(GFX_GLFW_ENABLED)

struct GLFWwindow;

#define glfwCreateWindowSurface ((VkResult (*)(void*, GLFWwindow*, const VkAllocationCallbacks*, uint64_t*))::getSym(DL_DEFAULT, "glfwCreateWindowSurface"))
#define glfwGetRequiredInstanceExtensions ((const char** (*)(uint32_t* count))::getSym(DL_DEFAULT, "glfwGetRequiredInstanceExtensions"))
#define glfwGetCocoaWindow ((id (*)(GLFWwindow*))::getSym(DL_DEFAULT, "glfwGetCocoaWindow"))

#endif // GFX_GLFW_ENABLED

#if defined (GFX_IMGUI_ENABLED)

#include "imgui.h" // IWYU pragma: keep

#endif // GFX_IMGUI_ENABLED

#if defined (GFX_BUILD_TRACY)

#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

#else

#define ZoneScoped
#define ZoneScopedN(x)

#define TracyCZoneN(c,x,y)
#define TracyCZoneEnd(c)

#endif // GFX_BUILD_TRACY

#endif // GRAPHICS_PCH_HPP
