/*
 * ---------------------------------------------------
 * imguiWidget.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/26 12:10:42
 * ---------------------------------------------------
 */

#include "imguiWidget.hpp"
#include "Entity.hpp"
#include "MaterialLibrary.hpp"
#include <cstring>
#include <imgui/imgui.h>

void editWidget(Material& material)
{
    ImGui::ColorEdit3("baseColor",     (float*)&material.baseColor);
    ImGui::ColorEdit3("specularColor", (float*)&material.specularColor);
    ImGui::ColorEdit3("emissiveColor", (float*)&material.emissiveColor);
    ImGui::DragFloat("shininess",      (float*)&material.shininess, 1, 1);
}

void editWidget(SubMesh& submesh)
{
    if (ImGui::BeginPopupContextItem("material selection popup"))
    {
        for (auto& material : MaterialLibrary::shared())
        {
            if (ImGui::Button(material->name))
                submesh.material = material;
        }
        ImGui::EndPopup();
    }

    ImGui::Text("vertexBuffer: %p", (void*)submesh.vertexBuffer);
    ImGui::Text("indexBuffer: %p",  (void*)submesh.indexBuffer);
    ImGui::Text("renderMethod: %p", (void*)submesh.renderMethod);
    ImGui::Text("Material");
    ImGui::SameLine();
    if(ImGui::Button("change"))
        ImGui::OpenPopup("material selection popup");
    ImGui::Indent(10);
        editWidget(*submesh.material);
    ImGui::Unindent(10);
}

void editWidget(Mesh& mesh)
{
    if (ImGui::TreeNode("Sub meshes"))
    {
        for (auto& subMesh : mesh.subMeshes)
        {
            ImGui::Text("Sub Mesh %p", &subMesh);
            ImGui::Indent(10);
                editWidget(subMesh);
            ImGui::Unindent(10);
        }
        ImGui::TreePop();
    }
}

void editWidget(Entity& entt)
{
    char buff[32];
    strncpy(buff, entt.name, 32); 
    ImGui::InputText("Name", buff, 32);
    entt.name = buff;

    ImGui::DragFloat3("position", (float*)&entt.position, 0.01);
    ImGui::DragFloat3("rotation", (float*)&entt.rotation, 0.01);

    if (PointLight* pointLight = dynamic_cast<PointLight*>(&entt))
    {
        ImGui::ColorEdit3("color",            (float*)&pointLight->color);
        ImGui::DragFloat("ambiantIntensity",  (float*)&pointLight->ambiantIntensity, 0.001, 0.0, 1.0);
        ImGui::DragFloat("diffuseIntensity",  (float*)&pointLight->diffuseIntensity, 0.001, 0.0, 1.0);
        ImGui::DragFloat("specularIntensity", (float*)&pointLight->specularIntensity, 0.001, 0.0, 1.0);
    }
    else if (RenderableEntity* renderableEntity = dynamic_cast<RenderableEntity*>(&entt))
    {
        editWidget(renderableEntity->mesh);
    }
}
