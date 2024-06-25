/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/25 11:29:39
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD_HPP
# define RENDERMETHOD_HPP

#include "Entity.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "MaterialLibrary.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

enum class VertexShader { universal3D };
enum class FragmentShader { baseTexture };

class IRenderMethod
{
public:
    struct Data
    {
        const math::mat4x4*             modelMatrix = nullptr;
        const math::mat4x4*             vpMatrix    = nullptr;
        const math::vec3f*              cameraPos   = nullptr;
        const utils::Array<PointLight>* pointLights = nullptr;
        const Material*                 material    = nullptr;
    };

public:
    virtual void use(const Data&) = 0;
};

template<VertexShader VS, FragmentShader FS>
class RenderMethod : public IRenderMethod
{
public:
    RenderMethod(const utils::SharedPtr<gfx::GraphicAPI>&);

    void use(const Data&) override;

    ~RenderMethod() = default;

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;
};

#endif // RENDERMETHOD_HPP