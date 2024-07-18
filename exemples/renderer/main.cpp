/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/17 16:01:05
 * ---------------------------------------------------
 */

#include "AssetManager.hpp"
#include "EntityComponent.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/Array.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);
    graphicAPI->initImgui();

    AssetManager::init(graphicAPI);

    Renderer renderer(window, graphicAPI);

    utils::Array<Entity> entities;
    utils::Array<Entity>::Iterator camera;

    {
        Entity entt;

        entt.name = "camera";

        entt.transformComponent = TransformComponent();
        entt.transformComponent->pos = { 0, 1, -4 };
        entt.transformComponent->rot = { 0.2, 0, 0 };
        entt.transformComponent->sca = { 1, 1, 1 };

        entt.viewPointComponent = ViewPointComponent { true };

        camera = entities.append(std::move(entt));
    }
    {
        Entity entt;

        entt.name = "Light1";

        entt.transformComponent = TransformComponent();
        entt.transformComponent->pos = { -1.5, 1.5, -2.0 };
        entt.transformComponent->rot = { 0, 0, 0 };
        entt.transformComponent->sca = { 0.1, 0.1, 0.1 };

        entt.lightSourceComponent = LightSourceComponent();
        entt.lightSourceComponent->type = LightSourceComponent::Type::point;

        entt.renderableComponent = RenderableComponent { AssetManager::shared().mesh<DefaultRenderMethod::Vertex>(RESSOURCES_DIR"/cube/cube.gltf")[0] };

        entities.append(std::move(entt));
    }
    {
        Entity entt;

        entt.name = "Light2";

        entt.transformComponent = TransformComponent();
        entt.transformComponent->pos = { 1.5, 1.5, 0.5 };
        entt.transformComponent->rot = { 0, 0, 0 };
        entt.transformComponent->sca = { 0.1, 0.1, 0.1 };

        entt.lightSourceComponent = LightSourceComponent();
        entt.lightSourceComponent->type = LightSourceComponent::Type::point;

        entt.renderableComponent = RenderableComponent { AssetManager::shared().mesh<DefaultRenderMethod::Vertex>(RESSOURCES_DIR"/cube/cube.gltf")[0] };

        entities.append(std::move(entt));
    }
    {
        Entity entt;

        entt.name = "cat";

        entt.transformComponent = TransformComponent();
        entt.transformComponent->pos = { -0.7, 0, -0.7 };
        entt.transformComponent->rot = { -PI/2, 2.5, 0.0 };
        entt.transformComponent->sca = {0.15, 0.15, 0.15};

        entt.renderableComponent = RenderableComponent { AssetManager::shared().mesh<DefaultRenderMethod::Vertex>(RESSOURCES_DIR"/cat/cat.gltf")[0] };

        entities.append(std::move(entt));
    }

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
    });
    
    while (running)
    {
        gfx::Platform::shared().pollEvents();

        if (window->isKeyPress(W_KEY))     camera->transformComponent->pos   += (math::mat4x4::rotation(camera->transformComponent->rot) * math::vec4f({0, 0, 1, 1})).xyz().normalized() * 0.2;
        if (window->isKeyPress(A_KEY))     camera->transformComponent->pos   -= (math::mat4x4::rotation(camera->transformComponent->rot) * math::vec4f({1, 0, 0, 1})).xyz().normalized() * 0.2;
        if (window->isKeyPress(S_KEY))     camera->transformComponent->pos   -= (math::mat4x4::rotation(camera->transformComponent->rot) * math::vec4f({0, 0, 1, 1})).xyz().normalized() * 0.2;
        if (window->isKeyPress(D_KEY))     camera->transformComponent->pos   += (math::mat4x4::rotation(camera->transformComponent->rot) * math::vec4f({1, 0, 0, 1})).xyz().normalized() * 0.2;
        if (window->isKeyPress(UP_KEY))    camera->transformComponent->rot.x -= 0.05;
        if (window->isKeyPress(LEFT_KEY))  camera->transformComponent->rot.y -= 0.05;
        if (window->isKeyPress(DOWN_KEY))  camera->transformComponent->rot.x += 0.05;
        if (window->isKeyPress(RIGHT_KEY)) camera->transformComponent->rot.y += 0.05;

        renderer.beginScene();
        {
            for (const auto& entity : entities)
                renderer.addEntity(entity);
        }
        renderer.endScene();
    }

    AssetManager::terminate();

    gfx::Platform::terminate();
    return 0;
}