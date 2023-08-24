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

#include "Entity.h"

namespace Util::Game::D3 {

Entity::Entity(uint32_t tag, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale) : Util::Game::Entity(tag), position(position), rotation(rotation), scale(scale) {}

const Math::Vector3D &Entity::getPosition() const {
    return position;
}

void Entity::setPosition(const Math::Vector3D &position) {
    Entity::position = position;
    onTransformChange();
}

void Entity::translate(const Math::Vector3D &translation) {
    setPosition(position + translation);
}

void Entity::translateLocal(const Math::Vector3D &translation) {
    translate(translation.rotate(getRotation()));
}

const Math::Vector3D &Entity::getRotation() const {
    return rotation;
}

void Entity::setRotation(const Math::Vector3D &rotation) {
    Entity::rotation = rotation % 360;
    onTransformChange();
}

void Entity::rotate(const Math::Vector3D &rotation) {
    setRotation(Entity::rotation + rotation);
}

const Math::Vector3D &Entity::getScale() const {
    return scale;
}

void Entity::setScale(const Math::Vector3D &scale) {
    Entity::scale = scale;
    onTransformChange();
}

void Entity::update(double delta) {
    onUpdate(delta);
}

}