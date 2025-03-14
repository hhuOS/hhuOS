/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "Entity.h"

namespace Util::Game::D3 {

Entity::Entity(uint32_t tag, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale) : Util::Game::Entity(tag), position(position), rotation(rotation), scale(scale), collider(Math::Vector3<double>(0, 0, 0), 0) {
    setRotation(rotation);
}

Entity::Entity(uint32_t tag, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale, const SphereCollider &collider) : Util::Game::Entity(tag), position(position), rotation(rotation), scale(scale), colliderPresent(true), collider(collider) {
    setRotation(rotation);
}

const Math::Vector3<double> &Entity::getPosition() const {
    return position;
}

void Entity::setPosition(const Math::Vector3<double> &position) {
    Entity::position = position;
}

void Entity::translate(const Math::Vector3<double> &translation) {
    setPosition(position + translation);
    if (colliderPresent) {
        collider.setPosition(position);
    }
}

void Entity::translateLocal(const Math::Vector3<double> &translation) {
    translate(translation.rotate(getRotation()));
}

const Math::Vector3<double> &Entity::getRotation() const {
    return rotation;
}

void Entity::setRotation(const Math::Vector3<double> &angle) {
    rotation = angle % 360;
}

void Entity::rotate(const Math::Vector3<double> &angle) {
    setRotation(rotation + angle);
}

const Math::Vector3<double> &Entity::getScale() const {
    return scale;
}

void Entity::setScale(const Math::Vector3<double> &scale) {
    Entity::scale = scale;
}

void Entity::update(double delta) {
    if (colliderPresent) {
        collider.setPosition(position);
    }

    onUpdate(delta);
}

bool Entity::hasCollider() const {
    return colliderPresent;
}

SphereCollider &Entity::getCollider() {
    return collider;
}

}