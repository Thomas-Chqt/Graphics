/*
 * ---------------------------------------------------
 * imguiWidget.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/26 12:09:27
 * ---------------------------------------------------
 */

#ifndef IMGUIWIDGET_HPP
# define IMGUIWIDGET_HPP

#include "Entity.hpp"
#include "MaterialLibrary.hpp"

void editWidget(Material&);
void editWidget(RenderableEntity::Mesh&);
void editWidget(Entity&);
void enttSelect(Entity& entt, Entity*& selectedEntt);

#endif // IMGUIWIDGET_HPP