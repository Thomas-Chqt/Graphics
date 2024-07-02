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

#include "AssetManager.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Window.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Entity.hpp"

class Renderer
{
public:
    Renderer(const utils::SharedPtr<gfx::Window>&, const utils::SharedPtr<gfx::GraphicAPI>&);

    inline void UI(const utils::Func<void()>& f) { m_makeUI = f; }

    void beginScene();
    void setCamera(const Camera&);
    void addPointLight(const PointLight&);
    void addMesh(const Mesh&, const math::mat4x4& transformMatrix);
    void endScene();

    ~Renderer();

private:
    utils::SharedPtr<gfx::Window> m_window;
    utils::SharedPtr<gfx::GraphicAPI> m_api;
    math::mat4x4 m_projectionMatrix;
    utils::Func<void()> m_makeUI;

    //scene datas
    const Camera* m_camera;
    utils::Array<const PointLight*> m_pointLights;
    utils::Dictionary<Material*, utils::Array<Mesh>> m_transformedMeshes;
};

#endif // RENDERER_HPP