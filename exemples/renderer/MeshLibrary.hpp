/*
 * ---------------------------------------------------
 * MeshLibrary.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 15:07:00
 * ---------------------------------------------------
 */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "MaterialLibrary.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"

struct Mesh
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    utils::SharedPtr<Material> material;
};

struct SubModel
{
    utils::String name;

    math::mat4x4 defaultMat;
    math::vec3f position = { 0.0, 0.0, 0.0 };
    math::vec3f rotation = { 0.0, 0.0, 0.0 };
    math::vec3f scale    = { 1.0, 1.0, 1.0 };
    
    utils::Array<Mesh> meshes;
    utils::Array<SubModel> subModels;

    inline math::mat4x4 modelMatrix() const { return math::mat4x4::translation(position) * math::mat4x4::rotation(rotation) * math::mat4x4::scale(scale) * defaultMat; }
};

struct Model
{
    utils::String name;
    utils::Array<Mesh> meshes;
    utils::Array<SubModel> subModels;
};

class ModelLibrary
{
public:
    ModelLibrary(const ModelLibrary&) = delete;
    ModelLibrary(ModelLibrary&&)      = delete;

    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<ModelLibrary>(new ModelLibrary(api)); }
    static inline ModelLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    Model modelFromFile(const utils::String& filePath);

    ~ModelLibrary() = default;

private:
    ModelLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api);

    static utils::UniquePtr<ModelLibrary> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::Dictionary<utils::String, Model> m_models;

public:
    ModelLibrary& operator = (const ModelLibrary&) = delete;
    ModelLibrary& operator = (ModelLibrary&&)      = delete;

};

#endif // LIBRARY_HPP