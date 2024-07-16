/*
 * ---------------------------------------------------
 * Renderer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 19:21:37
 * ---------------------------------------------------
 */

#ifndef RENDERER_HPP
# define RENDERER_HPP

#include "DirectionalLight.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Math/Matrix.hpp"
#include "Mesh.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/SharedPtr.hpp"

class Renderer
{
public:
    Renderer()                = delete;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;
    
    Renderer(const utils::SharedPtr<gfx::GraphicAPI>&, const utils::SharedPtr<gfx::Window>&);

    void render(const Mesh&, const Material&, const DirectionalLight&);

    ~Renderer();

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::Window> m_window;
    math::mat4x4 m_vpMatrix;

    utils::SharedPtr<RenderMethod> m_defautRenderMethod;

public:
    Renderer& operator = (const Renderer&) = delete;
    Renderer& operator = (Renderer&&)      = delete;
};

#endif // RENDERER_HPP