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

#include "EntityComponent.hpp"
#include "Graphics/BufferInstance.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Window.hpp"
#include "Math/Matrix.hpp"
#include "AssetManager.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"

class Renderer
{
public:
    Renderer()                = delete;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;

    Renderer(const utils::SharedPtr<gfx::Window>&, const utils::SharedPtr<gfx::GraphicAPI>&);

    void beginScene();

    void addEntity(const Entity&);

    void endScene();
    
    ~Renderer();

private:
    utils::SharedPtr<gfx::Window> m_window;
    utils::SharedPtr<gfx::GraphicAPI> m_api;
    
    math::mat4x4 m_projectionMatrix;
    gfx::BufferInstance<RenderMethod::MatrixBuffer> m_matrixBuffer;
    gfx::BufferInstance<RenderMethod::LightBuffer> m_lightBuffer;
    gfx::BufferInstance<RenderMethod::CameraBuffer> m_cameraBuffer;

    utils::Array<SubMesh> m_transformedSubmeshes;
};

#endif // RENDERER_HPP