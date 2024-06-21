/*
 * ---------------------------------------------------
 * Renderer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 10:34:43
 * ---------------------------------------------------
 */

#ifndef RENDERER_HPP
# define RENDERER_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Window.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Entity.hpp"

class Renderer
{
public:
    Renderer(const utils::SharedPtr<gfx::Window>&, const utils::SharedPtr<gfx::GraphicAPI>&);

    inline void beginScene(const Camera& camera) { m_camera = &camera; }

    inline void setDirectionalLight(const DirectionalLight& light) { m_directionalLight = &light; }
    void render(RenderableEntity&);

    inline void endScene() { m_directionalLight = nullptr; }

    ~Renderer();

private:
    utils::SharedPtr<gfx::Window> m_window;
    utils::SharedPtr<gfx::GraphicAPI> m_api;
    math::mat4x4 m_projectionMatrix;

    //scene datas
    const Camera* m_camera;
    const DirectionalLight* m_directionalLight;
};

#endif // RENDERER_HPP