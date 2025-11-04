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
 */

#include "Orientation.h"
#include "lib/util/math/Math.h"

namespace Pulsar::D3 {

const Util::Math::Vector3<double> Orientation::WORLD_UP = Util::Math::Vector3<double>(0, 1, 0);

Orientation::Orientation() {
    reset();
}

void Orientation::setRotation(const Util::Math::Vector3<double> &angle) {
    rotation = angle % 360;

    // If z is exactly 0, rotation behavior is weird (z-axis inverted) and I don't know why.
    // A dirty workaround is to set z to a very small value instead of 0.
    if (rotation.getZ() == 0) {
        rotation = Util::Math::Vector3<double>(rotation.getX(), rotation.getY(), 0.000001);
    }

    front = Util::Math::Vector3<double>(
            Util::Math::sine(Util::Math::toRadians(rotation.getZ())) * Util::Math::cosine(Util::Math::toRadians(rotation.getY())),
            Util::Math::sine(Util::Math::toRadians(rotation.getY())),
            -Util::Math::cosine(Util::Math::toRadians(rotation.getZ())) * Util::Math::cosine(Util::Math::toRadians(rotation.getY()))).normalize();
    right = front.crossProduct(WORLD_UP).normalize();
    up = right.crossProduct(front).normalize();
}

void Orientation::rotate(const Util::Math::Vector3<double> &angle) {
    setRotation(rotation + angle);
}

void Orientation::reset() {
    setRotation(Util::Math::Vector3<double>(0, 0, 0));
}

const Util::Math::Vector3<double>& Orientation::getRotation() const {
    return rotation;
}

const Util::Math::Vector3<double>& Orientation::getUp() const {
    return up;
}

const Util::Math::Vector3<double>& Orientation::getRight() const {
    return right;
}

const Util::Math::Vector3<double>& Orientation::getFront() const {
    return front;
}

void Orientation::setFront(const Util::Math::Vector3<double> &front) {
    const auto normalizedFront = front.normalize();
    const auto rotation = Util::Math::Vector3<double>(Orientation::rotation.getX(),
        Util::Math::toDegrees(Util::Math::arcsine(normalizedFront.getY())),
        Util::Math::toDegrees(Util::Math::arctangent2(normalizedFront.getX(), -normalizedFront.getZ())));

    setRotation(rotation);
}

}
