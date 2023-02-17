/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "lib/util/game/entity/component/Component.h"
#include "Entity.h"

namespace Util::Game {

Entity::Entity(const Math::Vector2D &position) : position(position) {}

void Entity::translate(const Math::Vector2D &translation) {
    velocity = velocity + translation;
}

void Entity::translateX(double x) {
    velocity = {x, velocity.getY()};
}

void Entity::translateY(double y) {
    position = {velocity.getX(), y};
}

void Entity::setPosition(const Math::Vector2D &position) {
    Entity::position = position;
}

void Entity::setVelocity(const Math::Vector2D &velocity) {
    Entity::velocity = velocity;
}

void Entity::addComponent(Component *component) {
    components.add(component);
}

const Util::Math::Vector2D& Entity::getPosition() const {
    return position;
}

const Math::Vector2D &Entity::getVelocity() const {
    return velocity;
}

void Entity::update(double delta) {
    for (auto *component : components) {
        component->update(delta);
    }
    
    onUpdate(delta);
}

}