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
 */

#include "RectangleCollider.h"

#include "lib/util/math/Math.h"
#include "lib/pulsar/Collider.h"
#include "lib/util/math/Vector2.h"
#include "lib/util/math/Vector3.h"

namespace Pulsar::D2 {

RectangleCollider::RectangleCollider(const Util::Math::Vector2<double> &position, const Util::Math::Vector2<double> &size, Collider::Type type) :
        Collider(position, type), size(size) {}

RectangleCollider::Side RectangleCollider::getOpposite(RectangleCollider::Side side) {
    switch (side) {
        case RIGHT:
            return LEFT;
        case LEFT:
            return RIGHT;
        case TOP:
            return BOTTOM;
        case BOTTOM:
            return TOP;
        default:
            return NONE;
    }
}

double RectangleCollider::getWidth() const {
    return size.getX();
}

double RectangleCollider::getHeight() const {
    return size.getY();
}

const Util::Math::Vector2<double> &RectangleCollider::getSize() const {
    return size;
}

void RectangleCollider::setWidth(double width) {
    size = Util::Math::Vector2<double>(width, size.getY());
}

void RectangleCollider::setHeight(double height) {
    size = Util::Math::Vector2<double>(size.getX(), height);
}

RectangleCollider::Side RectangleCollider::isColliding(const RectangleCollider &other) const {
    if (getPosition().getX() < other.getPosition().getX() + other.getWidth() &&
        getPosition().getX() + getWidth() > other.getPosition().getX() &&
        getPosition().getY() < other.getPosition().getY() + other.getHeight() &&
        getHeight() + getPosition().getY() > other.getPosition().getY()) {

        auto lastCenter = getPosition() + Util::Math::Vector2<double>(getWidth() / 2, getHeight() / 2);
        auto otherLastCenter = other.getPosition() + Util::Math::Vector2<double>(other.getWidth() / 2, other.getHeight() / 2);

        auto centerXDistance = lastCenter.getX() - otherLastCenter.getX();
        auto centerYDistance = lastCenter.getY() - otherLastCenter.getY();

        auto absoluteXDistance = getWidth() / 2 + other.getWidth() / 2 - Util::Math::absolute(centerXDistance);
        auto absoluteYDistance = getHeight() / 2 + other.getHeight() / 2 - Util::Math::absolute(centerYDistance);

        if (absoluteXDistance >= absoluteYDistance) {
            if (centerYDistance < 0) {
                return TOP;
            }

            return BOTTOM;
        }

        if (centerXDistance > 0) {
            return LEFT;
        }

        return RIGHT;
    }

    return NONE;
}

void RectangleCollider::setSize(const Util::Math::Vector2<double> &size) {
    RectangleCollider::size = size;
}

}