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
#include "MaterialLibrary.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Entity.hpp"

class Renderer
{
private:
    struct Renderable
    {
        math::mat4x4 modelMatrix;
        utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
        utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
        utils::SharedPtr<Material> material;
    };

public:
    Renderer(const utils::SharedPtr<gfx::Window>&, const utils::SharedPtr<gfx::GraphicAPI>&);

    void beginScene();
    inline void setImgui(const utils::Func<void()>& f) { m_imguiCalls = f; }
    void setCamera(const Camera&);
    void addPointLight(const PointLight&);
    void addRenderableEntity(const RenderableEntity&);
    void endScene();

    ~Renderer();

private:
    void useMaterial(const utils::SharedPtr<Material>&);

    utils::SharedPtr<gfx::Window> m_window;
    utils::SharedPtr<gfx::GraphicAPI> m_api;
    math::mat4x4 m_projectionMatrix;

    //scene datas
    const Camera* m_camera;
    utils::Func<void()> m_imguiCalls;
    utils::Array<const PointLight*> m_pointLights;
    utils::Dictionary<Material*, utils::Array<Renderable>> m_renderables;
};

#endif // RENDERER_HPP