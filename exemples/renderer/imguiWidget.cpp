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
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include <cstring>
#include <imgui/imgui.h>

void editWidget(Material& material)
{
    ImGui::Text("renderMethod: %p",    (void*)material.renderMethod);
    ImGui::Text("baseTexture: %p",     (void*)material.baseTexture);
    ImGui::Text("specularTexture: %p", (void*)material.specularTexture);
    ImGui::Text("emissiveTexture: %p", (void*)material.emissiveTexture);

    ImGui::ColorEdit3("baseColor##"     + utils::String::fromUInt((utils::uint64)&material), (float*)&material.baseColor);
    ImGui::ColorEdit3("specularColor##" + utils::String::fromUInt((utils::uint64)&material), (float*)&material.specularColor);
    ImGui::ColorEdit3("emissiveColor##" + utils::String::fromUInt((utils::uint64)&material), (float*)&material.emissiveColor);
    ImGui::DragFloat("shininess##"      + utils::String::fromUInt((utils::uint64)&material), (float*)&material.shininess, 1, 1);
}

void editWidget(RenderableEntity::Mesh& mesh)
{
    if (ImGui::BeginPopupContextItem("material selection popup##" + utils::String::fromUInt((utils::uint64)&mesh)))
    {
        for (auto& material : MaterialLibrary::shared())
        {
            if (ImGui::Button(material.val->name.isEmpty() ? "no name" : material.val->name))
                mesh.material = material.val;
        }
        ImGui::EndPopup();
    }
    ImGui::Text("vertexBuffer: %p", (void*)mesh.vertexBuffer);
    ImGui::Text("indexBuffer: %p",  (void*)mesh.indexBuffer);
    ImGui::Text("Material: %s", (char*)mesh.material->name);
    ImGui::SameLine();
    if(ImGui::Button("change##" + utils::String::fromUInt((utils::uint64)&mesh)))
        ImGui::OpenPopup("material selection popup##" + utils::String::fromUInt((utils::uint64)&mesh));
    ImGui::Indent(10);
    editWidget(*mesh.material);
    ImGui::Unindent(10);
};

void editWidget(Entity& entt)
{
    char buff[32];
    strncpy(buff, entt.name, 32); 
    ImGui::InputText("Name", buff, 32);
    entt.name = buff;

    ImGui::DragFloat3("position", (float*)&entt.position, 0.01);
    ImGui::DragFloat3("rotation", (float*)&entt.rotation, 0.01);
    ImGui::DragFloat3("scale",    (float*)&entt.scale,    0.01);

    if (PointLight* pointLight = dynamic_cast<PointLight*>(&entt))
    {
        ImGui::ColorEdit3("color",            (float*)&pointLight->color);
        ImGui::DragFloat("ambiantIntensity",  (float*)&pointLight->ambiantIntensity,  0.001, 0.0, 1.0);
        ImGui::DragFloat("diffuseIntensity",  (float*)&pointLight->diffuseIntensity,  0.001, 0.0, 1.0);
        ImGui::DragFloat("specularIntensity", (float*)&pointLight->specularIntensity, 0.001, 0.0, 1.0);
    }
    else if (RenderableEntity* renderableEntity = dynamic_cast<RenderableEntity*>(&entt))
    {
        for (auto& mesh : renderableEntity->meshes)
            editWidget(mesh);
    }
}

void enttSelect(Entity& entt, Entity*& selectedEntt)
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (selectedEntt == &entt)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    if (entt.subEntities.isEmpty())
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool node_open = ImGui::TreeNodeEx(&entt, node_flags, "%s", entt.name.isEmpty() ? "No name" : (char*)entt.name);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        selectedEntt = &entt;
    if (entt.subEntities.isEmpty() == false && node_open)
    {
        for (auto& subEntt : entt.subEntities)
            enttSelect(*subEntt, selectedEntt);
        ImGui::TreePop();
    }
}