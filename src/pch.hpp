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
#include <ctime>      // IWYU pragma: keep
#include <mutex>      // IWYU pragma: keep
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

#include "imgui.h"

// NOLINTBEGIN
#define GetCurrentContext() (reinterpret_cast<ImGuiContext*(*)()>(getSym(DL_DEFAULT, "GetCurrentContext"))())
#define GetIO() (*reinterpret_cast<ImGuiIO*(*)()>(getSym(DL_DEFAULT, "GetIO"))())
#define GetPlatformIO() (*reinterpret_cast<ImGuiPlatformIO*(*)()>(getSym(DL_DEFAULT, "GetPlatformIO"))())
#define GetMainViewport() (reinterpret_cast<ImGuiViewport*(*)()>(getSym(DL_DEFAULT, "GetMainViewport"))())
#define DebugCheckVersionAndDataLayout(...) (reinterpret_cast<bool(*)(const char*, size_t, size_t, size_t, size_t, size_t, size_t)>(getSym(DL_DEFAULT, "DebugCheckVersionAndDataLayout"))(__VA_ARGS__))
#define MemAlloc(...) (reinterpret_cast<void*(*)(size_t)>(getSym(DL_DEFAULT, "MemAlloc"))(__VA_ARGS__))
#define MemFree(...) (reinterpret_cast<void(*)(void*)>(getSym(DL_DEFAULT, "MemFree"))(__VA_ARGS__))
#define DestroyPlatformWindows() (reinterpret_cast<void(*)()>(getSym(DL_DEFAULT, "DestroyPlatformWindows"))())

#undef IMGUI_CHECKVERSION
#define IMGUI_CHECKVERSION() DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx))

#undef IM_ALLOC
#define IM_ALLOC(_SIZE) MemAlloc(_SIZE)

#undef IM_FREE
#define IM_FREE(_PTR) MemFree(_PTR)

#undef IM_NEW
#define IM_NEW(_TYPE) new(ImNewWrapper(), MemAlloc(sizeof(_TYPE))) _TYPE

#define IM_DELETE(ptr) delete ptr
// NOLINTEND

#endif // GFX_IMGUI_ENABLED

constexpr int MAX_FRAME_IN_FLIGHT = 3;

template<typename T>
using PerFrameInFlight = ext::array<T, MAX_FRAME_IN_FLIGHT>;

#endif // GRAPHICS_PCH_HPP
