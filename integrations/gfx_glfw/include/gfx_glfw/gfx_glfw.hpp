#pragma once

#include "Graphics/Instance.hpp"
#include "Graphics/Surface.hpp"

struct GLFWwindow;

namespace gfx
{

namespace glfw
{

const InstanceExtension* newInstanceExtension();
std::unique_ptr<Surface> createSurface(Instance&, GLFWwindow*);

} // namespace glfw

} // namespace gfx
