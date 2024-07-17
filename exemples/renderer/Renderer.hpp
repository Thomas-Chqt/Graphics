/*
 * ---------------------------------------------------
 * Renderer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/17 16:19:48
 * ---------------------------------------------------
 */

#ifndef RENDERER_HPP
# define RENDERER_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Window.hpp"
#include "Math/Matrix.hpp"

class Renderer
{
public:
    Renderer()                = delete;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;

    Renderer(const utils::SharedPtr<gfx::Window>&, const utils::SharedPtr<gfx::GraphicAPI>&);
    
    ~Renderer();

private:
    utils::SharedPtr<gfx::Window> m_window;
    utils::SharedPtr<gfx::GraphicAPI> m_api;

    math::mat4x4 m_projectionMatrix;
};

#endif // RENDERER_HPP