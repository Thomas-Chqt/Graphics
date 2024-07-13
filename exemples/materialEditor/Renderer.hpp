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
#include "Graphics/Buffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
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

    utils::SharedPtr<gfx::GraphicPipeline> m_defaultPipeline;
    utils::SharedPtr<gfx::Buffer> m_matrixBuffer;
    utils::SharedPtr<gfx::Buffer> m_materialBuffer;
    utils::SharedPtr<gfx::Buffer> m_lightBuffer;

public:
    Renderer& operator = (const Renderer&) = delete;
    Renderer& operator = (Renderer&&)      = delete;
};

#endif // RENDERER_HPP