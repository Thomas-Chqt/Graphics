/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 10:34:44
 * ---------------------------------------------------
 */

#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Math/Constants.hpp"
#include "Mesh.hpp"
#include "RenderMethod.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include <imgui.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

utils::SharedPtr<gfx::Texture> loadTexture(const gfx::GraphicAPI& api, const utils::String& path)
{
    int width = 0;
    int height = 0;
    stbi_uc* imgBytes = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);
    if (imgBytes == nullptr)
        throw utils::RuntimeError("Unable to load the texture for path: " + path);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api.newTexture(textureDescriptor);
    texture->replaceContent(imgBytes);

    stbi_image_free(imgBytes);

    return texture;
}

utils::SharedPtr<gfx::Texture> loadCubeMap(const gfx::GraphicAPI& api, const utils::Array<utils::String>& paths)
{
    int width = 0;
    int height = 0;

    stbi_uc* imgBytes[6];
    for (int i = 0; i < 6; i++)
    {
        imgBytes[i] = stbi_load(paths[i], &width, &height, nullptr, STBI_rgb_alpha);
        if (imgBytes[i] == nullptr)
            throw utils::RuntimeError("Unable to load the texture for path: " + paths[i]);
    }

    utils::SharedPtr<gfx::Texture> texture = api.newTexture(gfx::Texture::Descriptor::textureCubeDescriptor(width));

    for (int i = 0; i < 6; i++)
    {
        texture->replaceSliceContent(i, imgBytes[i]);
        stbi_image_free(imgBytes[i]);
    }

    return texture;
}

int main()
{
    gfx::Platform::init();
    {
        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);
        graphicAPI->initImgui();

        Renderer renderer(graphicAPI, window);

        utils::Array<Mesh> meshes;
        meshes.append(loadMeshes<PhongRenderMethod::Vertex>(*graphicAPI, RESSOURCES_DIR"/sphere.glb")[0]);
        meshes.append(loadMeshes<PhongRenderMethod::Vertex>(*graphicAPI, RESSOURCES_DIR"/cube.glb")[0]);

        Material material;

        DirectionalLight directionalLight;

        utils::Dictionary<utils::String, utils::SharedPtr<gfx::Texture>> textures;
        textures.insert("rock_diff", loadTexture(*graphicAPI, RESSOURCES_DIR"/rock_diff.jpg"));
        textures.insert("rock_nor", loadTexture(*graphicAPI, RESSOURCES_DIR"/rock_nor.jpg"));
        textures.insert("container_diffuse", loadTexture(*graphicAPI, RESSOURCES_DIR"/container_diffuse.png"));
        textures.insert("container_emissive", loadTexture(*graphicAPI, RESSOURCES_DIR"/container_emissive.png"));
        textures.insert("container_specular", loadTexture(*graphicAPI, RESSOURCES_DIR"/container_specular.png"));

        utils::SharedPtr<gfx::Texture> skyboxTexture = loadCubeMap(*graphicAPI, {
            utils::String(RESSOURCES_DIR) + "/skybox/px.png",
            utils::String(RESSOURCES_DIR) + "/skybox/nx.png",
            utils::String(RESSOURCES_DIR) + "/skybox/py.png",
            utils::String(RESSOURCES_DIR) + "/skybox/ny.png",
            utils::String(RESSOURCES_DIR) + "/skybox/pz.png",
            utils::String(RESSOURCES_DIR) + "/skybox/nz.png",
        });

        Camera camera;
        camera.distance = 3;

        bool running = true;
        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& keyDownEvent)
            {
                switch (keyDownEvent.keyCode())
                {
                case ESC_KEY:
                    running = false;
                    break;
                }
            });

            event.dispatch<gfx::ScrollEvent>([&](gfx::ScrollEvent& scrollEvent)
            {
                if (window->isKeyPress(LEFT_SHIFT_KEY))
                    camera.distance += (float)scrollEvent.offsetY();
                else
                {
                    camera.orientationX += (float)scrollEvent.offsetY() * (float)0.08;
                    camera.orientationY += (float)scrollEvent.offsetX() * (float)0.08;
                }
            });
        });

        Mesh* renderedMesh = &meshes[0];

        while (running)
        {
            gfx::Platform::shared().pollEvents();
            graphicAPI->beginFrame();
            {
                graphicAPI->beginImguiRenderPass();
                {
                    if (ImGui::Begin("Imgui", nullptr, ImGuiWindowFlags_MenuBar))
                    {
                        if (ImGui::BeginMenuBar())
                        {
                            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                            ImGui::EndMenuBar();
                        }
                        ImGui::SeparatorText("Light");
                        {
                            ImGui::ColorEdit3("Color",             (float*)&directionalLight.color);
                            ImGui::DragFloat ("ambiantIntensity",  (float*)&directionalLight.ambiantIntensity,  0.001, 0.0,     1.0);
                            ImGui::DragFloat ("diffuseIntensity",  (float*)&directionalLight.diffuseIntensity,  0.001, 0.0,     1.0);
                            ImGui::DragFloat ("specularIntensity", (float*)&directionalLight.specularIntensity, 0.001, 0.0,     1.0);
                            ImGui::DragFloat3("Rotation",          (float*)&directionalLight.rotation,          0.001, -(2*PI), 2*PI);
                        }
                        ImGui::SeparatorText("Mesh");
                        {
                            if (ImGui::BeginCombo("Mesh", renderedMesh->name, 0))
                            {
                                for (auto& mesh : meshes)
                                {
                                    if (ImGui::Selectable(mesh.name, renderedMesh == &mesh))
                                        renderedMesh = &mesh;
                                }
                                ImGui::EndCombo();
                            }
                        }
                        ImGui::SeparatorText("Material");
                        {
                            static utils::String selectedTexture_diffuse;
                            if (ImGui::BeginCombo("Diffuse texture", material.diffuse.texture ? selectedTexture_diffuse : "Use color", 0))
                            {
                                if (ImGui::Selectable("Use color", material.diffuse.texture == nullptr))
                                    material.diffuse.texture.clear();
                                for (auto& keyVal : textures)
                                {
                                    if (ImGui::Selectable(keyVal.key, material.diffuse.texture == keyVal.val))
                                    {
                                        material.diffuse.texture = keyVal.val;
                                        selectedTexture_diffuse = keyVal.key;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            if (material.diffuse.texture == nullptr)
                                ImGui::ColorEdit3("Diffuse color", (float*)&material.diffuse.value);
                            
                            ImGui::Spacing();

                            static utils::String selectedTexture_specular;
                            if (ImGui::BeginCombo("Specluar texture", material.specular.texture ? selectedTexture_specular : "Use color", 0))
                            {
                                if (ImGui::Selectable("Use color", material.specular.texture == nullptr))
                                    material.specular.texture.clear();
                                for (auto& keyVal : textures)
                                {
                                    if (ImGui::Selectable(keyVal.key, material.specular.texture == keyVal.val))
                                    {
                                        material.specular.texture = keyVal.val;
                                        selectedTexture_specular = keyVal.key;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            if (material.specular.texture == nullptr)
                                ImGui::ColorEdit3("Specular color", (float*)&material.specular.value);

                            ImGui::Spacing();

                            static utils::String selectedTexture_emissive;
                            if (ImGui::BeginCombo("Emissve texture", material.emissive.texture ? selectedTexture_emissive : "Use color", 0))
                            {
                                if (ImGui::Selectable("Use color", material.emissive.texture == nullptr))
                                    material.emissive.texture.clear();
                                for (auto& keyVal : textures)
                                {
                                    if (ImGui::Selectable(keyVal.key, material.emissive.texture == keyVal.val))
                                    {
                                        material.emissive.texture = keyVal.val;
                                        selectedTexture_emissive = keyVal.key;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            if (material.emissive.texture == nullptr)
                                ImGui::ColorEdit3("Emissive color", (float*)&material.emissive.value);

                            ImGui::Spacing();
                            ImGui::DragFloat("shininess", (float*)&material.shininess, 1, 1);
                            ImGui::Spacing();

                            static utils::String selectedNormalMap;
                            if (ImGui::BeginCombo("Normal map", material.normalMap ? selectedNormalMap : "Disabled", 0))
                            {
                                if (ImGui::Selectable("Disable", material.normalMap == nullptr))
                                    material.normalMap.clear();
                                for (auto& keyVal : textures)
                                {
                                    if (ImGui::Selectable(keyVal.key, material.normalMap == keyVal.val))
                                    {
                                        material.normalMap = keyVal.val;
                                        selectedNormalMap = keyVal.key;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                        }
                    }
                    ImGui::End();

                    renderer.beginScene(camera);
                    {
                        renderer.setLight(directionalLight);
                        renderer.renderMesh(*renderedMesh, material);
                        renderer.renderSkybox(skyboxTexture);
                    }
                    renderer.endScene();
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
    return 0;
}
