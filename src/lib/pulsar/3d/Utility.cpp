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

#include "Utility.h"

#include "lib/util/math/Math.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/pulsar/3d/Entity.h"
#include "lib/pulsar/3d/collider/SphereCollider.h"

namespace Pulsar::D3 {

Entity* Utility::findEntityUsingRaytrace(const Util::ArrayList<Entity*> &entities, Util::Math::Vector3<double> from, Util::Math::Vector3<double> direction, double length, double precision) {
    auto collider = SphereCollider(Util::Math::Vector3<double>(0, 0, 0), precision);
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

Util::Math::Vector3<double> Utility::findLookAt(const Util::Math::Vector3<double> &from, const Util::Math::Vector3<double> &to) {
    Util::Math::Vector3<double> v = to - from;
    Util::Math::Vector3<double> norm = v.normalize();

    auto x = norm.getX();
    auto y = norm.getY();
    auto z = norm.getZ();

    auto pitch = Util::Math::arcsine(y);
    auto a = x / Util::Math::cosine(pitch);

    // fix rounding errors (|a| shouldn't actually ever exceed 1)
    if (a > 1) a = 1;
    if (a < -1) a = -1;

    auto yaw = Util::Math::arcsine(a);
    auto c = 180 / Util::Math::PI_DOUBLE;

    yaw *= c;
    pitch *= c;

    // fix mirroring issue when getting vectors behind you
    if (z < 0) yaw = -180 - yaw;

    return { -pitch, yaw, 0 };
}

Util::Math::Vector3<double> Utility::findOrbitLocation(const Util::Math::Vector3<double> &start, const Util::Math::Vector3<double> &orbitCenter, const Util::Math::Vector3<double> &orbitAngle) {
    auto orbitAngleCleaned = Util::Math::Vector3<double>(orbitAngle.getX(), orbitAngle.getY(), 0);
    auto normAngleLocation = Util::Math::Vector3<double>(0, 0, 1).rotate(orbitAngleCleaned);
    auto radius = start.distance(orbitCenter);

    return orbitCenter + (normAngleLocation * radius);
}

Util::Math::Vector3<double> Utility::findOrbitLocationRelative(const Util::Math::Vector3<double> &start, const Util::Math::Vector3<double> &orbitCenter, const Util::Math::Vector3<double> &orbitAngleRelative) {
    auto currentOrbitAngle = findLookAt(orbitCenter, start);

    auto radius = start.distance(orbitCenter);
    auto orbitAngleCleaned = Util::Math::Vector3<double>(currentOrbitAngle.getX() + orbitAngleRelative.getX(), currentOrbitAngle.getY() + orbitAngleRelative.getY(), 0) % 360;
    auto normAngleLocation = Util::Math::Vector3<double>(0, 0, 1).rotate(orbitAngleCleaned);

    return normAngleLocation * radius;

}

}