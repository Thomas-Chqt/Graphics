/*
 * ---------------------------------------------------
 * Material.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/24 21:27:32
 * ---------------------------------------------------
 */

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "shaders/flat_color.slang"
#include "shaders/textured.slang"

#include <Graphics/Sampler.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/GraphicsPipeline.hpp>
#include <Graphics/ParameterBlock.hpp>
#include <Graphics/ParameterBlockPool.hpp>
#include <Graphics/Device.hpp>

#include <memory>

#include <glm/glm.hpp>

namespace scop
{

class Material
{
public:
    Material(const Material&) = delete;

    virtual const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const = 0;
    virtual std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool&) const = 0;

    virtual ~Material() = default;

protected:
    Material() = default;
    Material(Material&&) = default;

public:
    Material& operator=(const Material&) = delete;

protected:
    Material& operator=(Material&&) = default;
};

class FlatColorMaterial : public Material
{
public:
    FlatColorMaterial() = delete;
    FlatColorMaterial(const FlatColorMaterial&) = delete;
    FlatColorMaterial(FlatColorMaterial&&) = delete;

    FlatColorMaterial(gfx::Device& device); // need non const for gfx pipeline

    inline const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const override { return m_graphicsPipeline; }
    std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool& pool) const override;

    inline glm::vec4 diffuseColor() const { return m_materialData->content<shader::flat_color::MaterialData>()->diffuseColor; }
    inline void setDiffuseColor(const glm::vec4& d) { m_materialData->content<shader::flat_color::MaterialData>()->diffuseColor = d; }

    inline glm::vec3 specularColor() const { return m_materialData->content<shader::flat_color::MaterialData>()->specularColor; }
    inline void setSpecularColor(glm::vec3 s) { m_materialData->content<shader::flat_color::MaterialData>()->specularColor = s; }

    inline float shininess() const { return m_materialData->content<shader::flat_color::MaterialData>()->shininess; }
    inline void setShininess(float s) { m_materialData->content<shader::flat_color::MaterialData>()->shininess = s; }

    ~FlatColorMaterial() override = default;

private:
    inline static std::weak_ptr<gfx::GraphicsPipeline> s_graphicsPipeline;
    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;

    std::shared_ptr<gfx::Buffer> m_materialData;

public:
    FlatColorMaterial& operator=(const FlatColorMaterial&) = delete;
    FlatColorMaterial& operator=(FlatColorMaterial&&) = delete;
};

class TexturedMaterial : public Material
{
public:
    TexturedMaterial() = delete;
    TexturedMaterial(const TexturedMaterial&) = delete;
    TexturedMaterial(TexturedMaterial&&) = delete;

    TexturedMaterial(gfx::Device& device);

    inline const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const override { return m_graphicsPipeline; }
    std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool& pool) const override;

    inline const std::shared_ptr<gfx::Sampler>& sampler() const { return m_sampler; }
    inline void setSampler(const std::shared_ptr<gfx::Sampler>& s) { m_sampler = s; }

    inline glm::vec4 diffuseColor() const { return m_materialData->content<shader::textured::MaterialData>()->diffuseColor; }
    inline void setDiffuseColor(const glm::vec4& c) { m_materialData->content<shader::textured::MaterialData>()->diffuseColor = c; }

    inline const std::shared_ptr<gfx::Texture>& diffuseTexture() const { return m_diffuseTexture; }
    inline void setDiffuseTexture(const std::shared_ptr<gfx::Texture>& t) { m_diffuseTexture = t; }

    inline glm::vec3 specularColor() const { return m_materialData->content<shader::textured::MaterialData>()->specularColor; }
    inline void setSpecularColor(const glm::vec3& s) { m_materialData->content<shader::textured::MaterialData>()->specularColor = s; }

    inline float shininess() const { return m_materialData->content<shader::textured::MaterialData>()->shininess; }
    inline void setShininess(float s) { m_materialData->content<shader::textured::MaterialData>()->shininess = s; }

    inline glm::vec3 emissiveColor() const { return m_materialData->content<shader::textured::MaterialData>()->emissiveColor; }
    inline void setEmissiveColor(const glm::vec3& e) { m_materialData->content<shader::textured::MaterialData>()->emissiveColor = e; }

    inline const std::shared_ptr<gfx::Texture>& emissiveTexture() const { return m_emissiveTexture; }
    inline void setEmissiveTexture(const std::shared_ptr<gfx::Texture>& t) { m_emissiveTexture = t; }

    inline const std::shared_ptr<gfx::Texture>& normalTexture() const { return m_normalTexture; }
    inline void setNormalTexture(const std::shared_ptr<gfx::Texture>& t) { m_normalTexture = t; }

    ~TexturedMaterial() override = default;

private:
    inline static std::weak_ptr<gfx::GraphicsPipeline> s_graphicsPipeline;
    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;

    std::shared_ptr<gfx::Sampler> m_sampler;
    std::shared_ptr<gfx::Texture> m_diffuseTexture;
    std::shared_ptr<gfx::Texture> m_emissiveTexture;
    std::shared_ptr<gfx::Texture> m_normalTexture;
    std::shared_ptr<gfx::Buffer> m_materialData;

public:
    TexturedMaterial& operator=(const TexturedMaterial&) = delete;
    TexturedMaterial& operator=(TexturedMaterial&&) = delete;
};

} // namespace scop

#endif // MATERIAL_HPP
