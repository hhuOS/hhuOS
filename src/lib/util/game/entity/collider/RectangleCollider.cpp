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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#include "RectangleCollider.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/entity/collider/Collider.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Game {

RectangleCollider::RectangleCollider(const Math::Vector2D &position, Collider::Type type, double width, double height) :
        Collider(position, type), width(width), height(height) {}

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
    return width;
}

double RectangleCollider::getHeight() const {
    return height;
}

void RectangleCollider::setWidth(double width) {
    RectangleCollider::width = width;
}

void RectangleCollider::setHeight(double height) {
    RectangleCollider::height = height;
}

RectangleCollider::Side RectangleCollider::isColliding(const RectangleCollider &other) const {
    if (getPosition().getX() < other.getPosition().getX() + other.getWidth() &&
        getPosition().getX() + getWidth() > other.getPosition().getX() &&
        getPosition().getY() < other.getPosition().getY() + other.getHeight() &&
        getHeight() + getPosition().getY() > other.getPosition().getY()) {

        auto lastCenter = lastPosition + Math::Vector2D(getWidth() / 2, getHeight() / 2);
        auto otherLastCenter = other.lastPosition + Math::Vector2D(other.getWidth() / 2, other.getHeight() / 2);

        auto centerXDistance = lastCenter.getX() - otherLastCenter.getX();
        auto centerYDistance = lastCenter.getY() - otherLastCenter.getY();

        auto absoluteXDistance = getWidth() / 2 + other.getWidth() / 2 - Math::absolute(centerXDistance);
        auto absoluteYDistance = getHeight() / 2 + other.getHeight() / 2 - Math::absolute(centerYDistance);

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

}