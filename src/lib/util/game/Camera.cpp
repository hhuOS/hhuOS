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

#include "Camera.h"

#include "lib/util/math/Vector2.h"
#include "lib/util/math/Math.h"

namespace Util::Game {

Camera::Camera() {
    reset();
}

const Math::Vector3<double>& Camera::getPosition() const {
    return position;
}

const Math::Vector3<double>& Camera::getRotation() const {
    return rotation;
}

const Math::Vector3<double>& Camera::getTargetVector() const {
    return targetVector;
}

const Math::Vector3<double>& Camera::getRightVector() const {
    return rightVector;
}

void Camera::setPosition(const Math::Vector3<double> &position) {
    Camera::position = position;
}

void Camera::setPosition(const Math::Vector2<double> &position) {
    Camera::position = Math::Vector3<double>(position.getX(), position.getY(), 0);
}

void Camera::setRotation(const Math::Vector3<double> &angle) {
    rotation = angle % 360;

    targetVector = Util::Math::Vector3<double>(
            Util::Math::sine(Util::Math::toRadians(rotation.getZ())) * Util::Math::cosine(Util::Math::toRadians(rotation.getY())),
            Util::Math::sine(Util::Math::toRadians(rotation.getY())),
            -Util::Math::cosine(Util::Math::toRadians(rotation.getZ())) * Util::Math::cosine(Util::Math::toRadians(rotation.getY()))).normalize();
    rightVector = targetVector.cross(Util::Math::Vector3<double>(0, 1, 0)).normalize();
}

void Camera::translate(const Math::Vector3<double> &translation) {
    position = position + translation;
}

void Camera::rotate(const Math::Vector3<double> &angle) {
    setRotation(rotation + angle);
}

void Camera::reset() {
    position = { 0.0, 0.0, 0.0 };
    rotation = { 0.0, 0.0, 0.0 };

    targetVector = { 0.0, 0.0, -1.0 };
    rightVector = { 1.0f, 0.0f, 0.0f };
}

}