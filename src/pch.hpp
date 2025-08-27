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

#if defined(GFX_USE_UTILSCPP)

namespace ext = utl;  // NOLINT

#else // GFX_USE_UTILSCPP

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
namespace ext = std;  // NOLINT

#endif // GFX_USE_UTILSCPP

#if defined(GFX_BUILD_METAL)
#if defined(__OBJC__)

#import <TargetConditionals.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <AppKit/NSWindow.h>

#else // __OBJC__

template<typename T> using id = T*;
inline constexpr auto nil = nullptr;
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

#endif // __OBJC__
#endif // GFX_BUILD_METAL

#if defined (GFX_BUILD_VULKAN)

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

#endif // GFX_BUILD_VULKAN

#if defined(GFX_GLFW_ENABLED)

#include <dlLoad/dlLoad.h>
struct GLFWwindow;
#define glfwCreateWindowSurface ((VkResult (*)(void*, GLFWwindow*, const VkAllocationCallbacks*, uint64_t*))::getSym(DL_DEFAULT, "glfwCreateWindowSurface"))
#define glfwGetRequiredInstanceExtensions ((const char** (*)(uint32_t* count))::getSym(DL_DEFAULT, "glfwGetRequiredInstanceExtensions"))
#define glfwGetCocoaWindow ((id (*)(GLFWwindow*))::getSym(DL_DEFAULT, "glfwGetCocoaWindow"))

#endif // GFX_GLFW_ENABLED

#if defined (GFX_IMGUI_ENABLED)

struct ImDrawData;

#endif // GFX_IMGUI_ENABLED

constexpr int MAX_FRAME_IN_FLIGHT = 3;

template<typename T>
using PerFrameInFlight = ext::array<T, MAX_FRAME_IN_FLIGHT>;

#endif // GRAPHICS_PCH_HPP
