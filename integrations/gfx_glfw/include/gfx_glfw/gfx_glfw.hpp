#pragma once

#include "Graphics/Instance.hpp"
#include "Graphics/Surface.hpp"

struct GLFWwindow;

namespace gfx::glfw
{

const InstanceExtension* getInstanceExtension();
std::unique_ptr<Surface> createSurface(Instance&, GLFWwindow*);

} // namespace gfx::glfw
