/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/22 10:31:15
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD_HPP
# define RENDERMETHOD_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Texture.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "ShaderDatas.hpp"
#include "UtilsCPP/SharedPtr.hpp"

class RenderMethod
{
public:
    struct Data
    {
        const math::mat4x4* modelMatrix                      = nullptr;
        const math::mat4x4* vpMatrix                         = nullptr;
        const math::vec3f* cameraPos                         = nullptr;
        const utils::Array<PointLight>* pointLights          = nullptr;
        const Material* material                             = nullptr;
        const utils::SharedPtr<gfx::Texture>* diffuseTexture = nullptr;
    };

public:
    virtual void use(gfx::GraphicAPI&, const Data&) = 0;

    virtual ~RenderMethod() = default;

protected:
    RenderMethod() = default;

    utils::SharedPtr<gfx::GraphicPipeline> m_pipeline;
};

class FlatColorRenderMethod : public RenderMethod
{
public:
    FlatColorRenderMethod(const gfx::GraphicAPI&);

    void use(gfx::GraphicAPI&, const Data&) override;

    ~FlatColorRenderMethod() override = default;
};

class LightCubeRenderMethod : public RenderMethod
{
public:
    LightCubeRenderMethod(const gfx::GraphicAPI&);

    void use(gfx::GraphicAPI&, const Data&) override;

    ~LightCubeRenderMethod() override = default;
};

class TexturedRenderMethod : public RenderMethod
{
public:
    TexturedRenderMethod(const gfx::GraphicAPI&);

    void use(gfx::GraphicAPI&, const Data&) override;

    ~TexturedRenderMethod() override = default;
};

#endif // RENDERMETHOD_HPP