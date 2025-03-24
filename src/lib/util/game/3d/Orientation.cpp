/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Orientation.h"

#include <lib/util/math/Math.h>

namespace Util::Game::D3 {

const Math::Vector3<double> Orientation::WORLD_UP = Math::Vector3<double>(0, 1, 0);

Orientation::Orientation() {
    reset();
}

void Orientation::setRotation(const Math::Vector3<double> &angle) {
    rotation = angle % 360;

    // If z is exactly 0, rotation behavior is weird (z-axis inverted) and I don't know why.
    // A dirty workaround is to set z to a very small value instead of 0.
    if (rotation.getZ() == 0) {
        rotation = Math::Vector3<double>(rotation.getX(), rotation.getY(), 0.000001);
    }

    front = Math::Vector3<double>(
            Math::sine(Math::toRadians(rotation.getZ())) * Math::cosine(Math::toRadians(rotation.getY())),
            Math::sine(Math::toRadians(rotation.getY())),
            -Math::cosine(Math::toRadians(rotation.getZ())) * Math::cosine(Math::toRadians(rotation.getY()))).normalize();
    right = front.cross(WORLD_UP).normalize();
    up = right.cross(front).normalize();
}

void Orientation::rotate(const Math::Vector3<double> &angle) {
    setRotation(rotation + angle);
}

void Orientation::reset() {
    setRotation(Math::Vector3<double>(0, 0, 0));
}

const Math::Vector3<double>& Orientation::getRotation() const {
    return rotation;
}

const Math::Vector3<double>& Orientation::getUp() const {
    return up;
}

const Math::Vector3<double>& Orientation::getRight() const {
    return right;
}

const Math::Vector3<double>& Orientation::getFront() const {
    return front;
}

void Orientation::setFront(const Math::Vector3<double> &front) {
    const auto normalizedFront = front.normalize();
    const auto rotation = Math::Vector3<double>(
        Orientation::rotation.getX(),
        Math::toDegrees(Math::arcsine(normalizedFront.getY())),
        Math::toDegrees(Math::arctangent2(normalizedFront.getX(), -normalizedFront.getZ()))
        );

    setRotation(rotation);
}

}
