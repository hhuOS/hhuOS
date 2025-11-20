/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Entity.h"

#include "pulsar/Entity.h"
#include "pulsar/3d/Orientation.h"

namespace Pulsar::D3 {

Entity::Entity(const size_t tag, const Util::Math::Vector3<double> &position,
    const Util::Math::Vector3<double> &rotation, const Util::Math::Vector3<double> &scale,
    const SphereCollider &collider) : Pulsar::Entity(tag), position(position), scale(scale), collider(collider)
{
    setRotation(rotation);
}

void Entity::onCollisionEvent([[maybe_unused]] const CollisionEvent &event) {}

const Util::Math::Vector3<double> &Entity::getPosition() const {
    return position;
}

void Entity::setPosition(const Util::Math::Vector3<double> &position) {
    Entity::position = position;
}

void Entity::translate(const Util::Math::Vector3<double> &translation) {
    setPosition(position + translation);

    if (hasCollider()) {
        collider.setPosition(position);
    }
}

const Util::Math::Vector3<double>& Entity::getUpVector() const {
    return orientation.getUp();
}

const Util::Math::Vector3<double>& Entity::getRightVector() const {
    return orientation.getRight();
}

const Util::Math::Vector3<double>& Entity::getFrontVector() const {
    return orientation.getFront();
}

void Entity::translateLocal(const Util::Math::Vector3<double> &translation) {
    translate(translation.rotate(getRotation()));
}

const Orientation& Entity::getOrientation() const {
    return orientation;
}

const Util::Math::Vector3<double> &Entity::getRotation() const {
    return orientation.getRotation();
}

void Entity::setFrontVector(const Util::Math::Vector3<double> &front) {
    orientation.setFront(front);
}

void Entity::setRotation(const Util::Math::Vector3<double> &angle) {
    orientation.setRotation(angle);
}

void Entity::rotate(const Util::Math::Vector3<double> &angle) {
    orientation.rotate(angle);
}

const Util::Math::Vector3<double> &Entity::getScale() const {
    return scale;
}

void Entity::setScale(const Util::Math::Vector3<double> &scale) {
    Entity::scale = scale;
}

void Entity::update(const double delta) {
    if (hasCollider()) {
        collider.setPosition(position);
    }

    onUpdate(delta);
}

bool Entity::hasCollider() const {
    return collider.getRadius() != 0;
}

SphereCollider &Entity::getCollider() {
    return collider;
}

}