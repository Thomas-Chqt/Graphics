/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/30 09:33:39
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD
# define RENDERMETHOD

#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"

enum class Shader { universal3D, baseTexture, baseColor };

class PointLight;
class Material;

class IRenderMethod
{
public:
    virtual void use() = 0;

    virtual void setVpMatrix(const math::mat4x4&) = 0;
    virtual void setModelMatrix(const math::mat4x4&) = 0;
    virtual void setCameraPos(const math::vec3f&) = 0;
    virtual void setPointLights(const utils::Array<const PointLight*>&) = 0;
    virtual void setMaterial(const Material&) = 0;

    virtual ~IRenderMethod() = default;

protected:
    IRenderMethod() = default;
};

template<Shader VS, Shader FS>
class RenderMethod : public IRenderMethod
{
public:
    RenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api);

    void use() override
    {
        m_graphicAPI->useGraphicsPipeline(m_graphicPipeline);
    }

    void setVpMatrix(const math::mat4x4&) override;
    void setModelMatrix(const math::mat4x4&) override;
    void setCameraPos(const math::vec3f&) override;
    void setPointLights(const utils::Array<const PointLight*>&) override;
    void setMaterial(const Material&) override;

    ~RenderMethod() override = default;

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;
};

#endif // RENDERMETHOD