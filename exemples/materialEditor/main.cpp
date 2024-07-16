/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 10:34:44
 * ---------------------------------------------------
 */

#include "DirectionalLight.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Math/Constants.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "imgui/imgui.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

utils::SharedPtr<gfx::Texture> loadTexture(const gfx::GraphicAPI& api, const utils::String& path)
{
    int width = 0;
    int height = 0;
    stbi_uc* imgBytes = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api.newTexture(textureDescriptor);
    texture->replaceContent(imgBytes);

    stbi_image_free(imgBytes);

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

        Mesh sphere = loadMeshes(*graphicAPI, RESSOURCES_DIR"/sphere.glb")[0];
        Material material;
        DirectionalLight directionalLight;
        utils::Dictionary<utils::String, utils::SharedPtr<gfx::Texture>> textures;
        textures.insert("rock_diff", loadTexture(*graphicAPI, RESSOURCES_DIR"/rock_diff.jpg"));
        textures.insert("rock_nor", loadTexture(*graphicAPI, RESSOURCES_DIR"/rock_nor.jpg"));

        bool running = true;
        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& keyDownEvent) {
                if (keyDownEvent.keyCode() == ESC_KEY) running = false;
            });
        });

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
                        ImGui::SeparatorText("Material");
                        {
                            ImGui::Text("Diffuse");
                            ImGui::Indent(10);
                            {
                                static utils::String selectedTexture_diffuse = "Value";
                                if (ImGui::BeginCombo("Texture##1", selectedTexture_diffuse, 0))
                                {
                                    const bool is_selected = (selectedTexture_diffuse == utils::String("Value"));
                                    if (ImGui::Selectable("Value", is_selected))
                                    {
                                        selectedTexture_diffuse = "Value";
                                        material.diffuse.texture.clear();
                                    }
                                    for (auto& keyVal : textures)
                                    {
                                        const bool is_selected = (selectedTexture_diffuse == keyVal.key);
                                        if (ImGui::Selectable(keyVal.key, is_selected))
                                        {
                                            selectedTexture_diffuse = keyVal.key;
                                            material.diffuse.texture = keyVal.val;
                                        }
                                    }
                                    ImGui::EndCombo();
                                }
                                if (selectedTexture_diffuse == utils::String("Value"))
                                    ImGui::ColorEdit3("value##1", (float*)&material.diffuse.value);
                            }
                            ImGui::Unindent(10);

                            ImGui::Text("Specular");
                            ImGui::Indent(10);
                                ImGui::ColorEdit3("value##2", (float*)&material.specular.value);
                            ImGui::Unindent(10);

                            ImGui::Text("Emissive");
                            ImGui::Indent(10);
                                ImGui::ColorEdit3("value##3", (float*)&material.emissive.value);
                            ImGui::Unindent(10);
                            
                            ImGui::DragFloat ("shininess", (float*)&material.shininess, 1, 1);
                        }
                    }
                    ImGui::End();

                    renderer.render(sphere, material, directionalLight);
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
    return 0;
}