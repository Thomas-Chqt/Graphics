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

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

enum class Shader { universal3D, baseTexture, baseColor };

class PointLight;
class Material;

class IRenderMethod
{
public:
    struct Uniforms
    {
        const math::mat4x4&                    modelMatrix;
        const math::mat4x4&                    vpMatrix;
        const math::vec3f&                     cameraPos;
        const utils::Array<const PointLight*>& pointLights;
        const Material&                        material;
    };

public:
    virtual void use(const Uniforms&) = 0;
    virtual ~IRenderMethod() = default;

protected:
    IRenderMethod() = default;
};

template<Shader S>
void addToDescriptor(gfx::GraphicPipeline::Descriptor&);

template<Shader S>
void setUniforms(gfx::GraphicAPI&, const IRenderMethod::Uniforms&);

template<Shader VS, Shader FS>
class RenderMethod : public IRenderMethod
{
public:
    RenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_graphicAPI(api)
    {
        gfx::GraphicPipeline::Descriptor descriptor;
        addToDescriptor<VS>(descriptor);
        addToDescriptor<FS>(descriptor);
        m_graphicPipeline = m_graphicAPI->newGraphicsPipeline(descriptor);
    }

    void use(const Uniforms& uniforms) override
    {
        m_graphicAPI->useGraphicsPipeline(m_graphicPipeline);
        setUniforms<VS>(*m_graphicAPI, uniforms);
        setUniforms<FS>(*m_graphicAPI, uniforms);
    }

    ~RenderMethod() override = default;

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;
};

#endif // RENDERMETHOD_HPP