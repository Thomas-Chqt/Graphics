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

#include "Camera.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
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

    void beginScene(const Camera&);
    inline void setLight(const DirectionalLight& light) { m_light = &light; }
    void renderMesh(const Mesh&, const Material&);
    void renderSkybox(const utils::SharedPtr<gfx::Texture>&);
    inline void endScene() {}

    ~Renderer();

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::Window> m_window;

    PhongRenderMethod m_phongRenderMethod;
    SkyboxRenderMethod m_skyBoxRenderMethod;

    utils::SharedPtr<gfx::Buffer> m_skyBoxVertexBuffer;
    math::mat4x4 m_projectionMatrix;

    // Scene data
    math::vec3f m_cameraPos;
    math::mat4x4 m_viewMatrix;
    const DirectionalLight* m_light;

public:
    Renderer& operator = (const Renderer&) = delete;
    Renderer& operator = (Renderer&&)      = delete;
};

#endif // RENDERER_HPP