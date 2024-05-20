/*
 * ---------------------------------------------------
 * OpenGLGraphicAPI.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 14:42:15
 * ---------------------------------------------------
 */

#ifndef OPENGLGRAPHICAPI_HPP
# define OPENGLGRAPHICAPI_HPP

#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
private:
    friend utils::SharedPtr<GraphicAPI> GraphicAPI::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

public:
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

    void setClearColor(float r, float g, float b, float a) override;

    void beginFrame() override;    
    void endFrame() override;

    ~OpenGLGraphicAPI() override = default;

private:
    OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<Window> m_renderTarget;
    float m_clearColor[4] = {};

public:
    OpenGLGraphicAPI operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP