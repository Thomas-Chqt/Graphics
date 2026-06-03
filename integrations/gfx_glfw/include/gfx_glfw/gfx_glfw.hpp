#pragma once

#include "Graphics/Surface.hpp"

#include <memory>

struct GLFWwindow;

namespace gfx
{

class Instance;

namespace glfw
{

std::unique_ptr<Surface> createSurface(Instance&, GLFWwindow*);

} // namespace glfw

} // namespace gfx
