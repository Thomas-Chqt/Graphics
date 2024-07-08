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

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Math/Matrix.hpp"
#include "Mesh.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "DirectionalLight.hpp"

class Renderer
{
public:
    Renderer(const utils::SharedPtr<gfx::GraphicAPI>&, const utils::SharedPtr<gfx::Window>&);

    void render(const Mesh&, const DirectionalLight&, const Material&);

    ~Renderer();

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::Window> m_window;
    math::mat4x4 m_vpMatrix;

    utils::SharedPtr<gfx::GraphicPipeline> m_defaultPipeline;
};

#endif // RENDERER_HPP