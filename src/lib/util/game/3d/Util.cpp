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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/risch114/bachelorarbeit
 */

#include "Util.h"
#include "lib/util/math/Math.h"

namespace Util::Game::D3 {

Entity* Util::findEntityUsingRaytrace(const ArrayList<Entity*> &entities, Math::Vector3D from, Math::Vector3D direction, double length, double precision) {
    auto collider = SphereCollider(Math::Vector3D(0, 0, 0), precision);
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

Math::Vector3D Util::findLookAt(const Math::Vector3D &from, const Math::Vector3D &to) {
    Math::Vector3D v = to - from;
    Math::Vector3D norm = v.normalize();

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

Math::Vector3D Util::findOrbitLocation(const Math::Vector3D &start, const Math::Vector3D &orbitCenter, const Math::Vector3D &orbitAngle) {
    auto orbitAngleCleaned = Math::Vector3D(orbitAngle.getX(), orbitAngle.getY(), 0);
    auto normAngleLocation = Math::Vector3D(0, 0, 1).rotate(orbitAngleCleaned);
    auto radius = start.distance(orbitCenter);

    return orbitCenter + (normAngleLocation * radius);
}

Math::Vector3D Util::findOrbitLocationRelative(const Math::Vector3D &start, const Math::Vector3D &orbitCenter, const Math::Vector3D &orbitAngleRelative) {
    auto currentOrbitAngle = findLookAt(orbitCenter, start);

    auto radius = start.distance(orbitCenter);
    auto orbitAngleCleaned = Math::Vector3D(currentOrbitAngle.getX() + orbitAngleRelative.getX(), currentOrbitAngle.getY() + orbitAngleRelative.getY(), 0) % 360;
    auto normAngleLocation = Math::Vector3D(0, 0, 1).rotate(orbitAngleCleaned);

    return normAngleLocation * radius;

}

}