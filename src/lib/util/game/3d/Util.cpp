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
 */

#include "Util.h"

#include "lib/util/math/Math.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/3d/Entity.h"
#include "lib/util/game/3d/collider/SphereCollider.h"

namespace Util::Game::D3 {

Entity* Util::findEntityUsingRaytrace(const ArrayList<Entity*> &entities, Math::Vector3<double> from, Math::Vector3<double> direction, double length, double precision) {
    auto collider = SphereCollider(Math::Vector3<double>(0, 0, 0), precision);
    auto normalizedDirection = direction.normalize();

    for (double x = 0; x < length; x += precision) {
        auto position = from + (normalizedDirection * x);
        collider.setPosition(position);

        for (auto *entity : entities) {
            if (entity->hasCollider()) {
                auto entityCollider = entity->getCollider();
                if (collider.isColliding(entityCollider)) {
                    return entity;
                }
            }
        }
    }

    return nullptr;

}

Math::Vector3<double> Util::findLookAt(const Math::Vector3<double> &from, const Math::Vector3<double> &to) {
    Math::Vector3<double> v = to - from;
    Math::Vector3<double> norm = v.normalize();

    auto x = norm.getX();
    auto y = norm.getY();
    auto z = norm.getZ();

    auto pitch = Math::arcsine(y);
    auto a = x / Math::cosine(pitch);

    // fix rounding errors (|a| shouldn't actually ever exceed 1)
    if (a > 1) a = 1;
    if (a < -1) a = -1;

    auto yaw = Math::arcsine(a);
    auto c = 180 / Math::PI;

    yaw *= c;
    pitch *= c;

    // fix mirroring issue when getting vectors behind you
    if (z < 0) yaw = -180 - yaw;

    return { -pitch, yaw, 0 };
}

Math::Vector3<double> Util::findOrbitLocation(const Math::Vector3<double> &start, const Math::Vector3<double> &orbitCenter, const Math::Vector3<double> &orbitAngle) {
    auto orbitAngleCleaned = Math::Vector3<double>(orbitAngle.getX(), orbitAngle.getY(), 0);
    auto normAngleLocation = Math::Vector3<double>(0, 0, 1).rotate(orbitAngleCleaned);
    auto radius = start.distance(orbitCenter);

    return orbitCenter + (normAngleLocation * radius);
}

Math::Vector3<double> Util::findOrbitLocationRelative(const Math::Vector3<double> &start, const Math::Vector3<double> &orbitCenter, const Math::Vector3<double> &orbitAngleRelative) {
    auto currentOrbitAngle = findLookAt(orbitCenter, start);

    auto radius = start.distance(orbitCenter);
    auto orbitAngleCleaned = Math::Vector3<double>(currentOrbitAngle.getX() + orbitAngleRelative.getX(), currentOrbitAngle.getY() + orbitAngleRelative.getY(), 0) % 360;
    auto normAngleLocation = Math::Vector3<double>(0, 0, 1).rotate(orbitAngleCleaned);

    return normAngleLocation * radius;

}

}