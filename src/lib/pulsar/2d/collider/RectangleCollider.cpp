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

#include "RectangleCollider.h"

#include "util/math/Math.h"
#include "util/math/Vector2.h"
#include "util/math/Vector3.h"
#include "pulsar/Collider.h"

namespace Pulsar {
namespace D2 {

RectangleCollider::Side RectangleCollider::isColliding(const RectangleCollider &other) const {
    if (getPosition().getX() < other.getPosition().getX() + other.getWidth() &&
        getPosition().getX() + getWidth() > other.getPosition().getX() &&
        getPosition().getY() < other.getPosition().getY() + other.getHeight() &&
        getHeight() + getPosition().getY() > other.getPosition().getY()) {

        const auto lastCenter = getPosition() +
            Util::Math::Vector2<float>(getWidth() / 2, getHeight() / 2);
        const auto otherLastCenter = other.getPosition() +
            Util::Math::Vector2<float>(other.getWidth() / 2, other.getHeight() / 2);

        const auto centerXDistance = lastCenter.getX() - otherLastCenter.getX();
        const auto centerYDistance = lastCenter.getY() - otherLastCenter.getY();

        const auto absoluteXDistance = getWidth() / 2 +
            other.getWidth() / 2 - Util::Math::absolute(centerXDistance);
        const auto absoluteYDistance = getHeight() / 2 +
            other.getHeight() / 2 - Util::Math::absolute(centerYDistance);

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
}