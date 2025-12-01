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

#ifndef HHUOS_LIB_PULSAR_3D_SPHERECOLLIDER_H
#define HHUOS_LIB_PULSAR_3D_SPHERECOLLIDER_H

#include "pulsar/Collider.h"
#include "util/math/Vector3.h"

namespace Pulsar::D3 {

/// A spherical collider for 3D collision detection.
/// Collisions with other sphere colliders can be detected by calling `isColliding()`.
/// The sphere collider is defined by its position and radius.
class SphereCollider : public Collider {

public:
    /// Create a new sphere collider instance with radius 0 at the origin.
    /// An entity with such a collider is considered to have no collider.
    SphereCollider() = default;

    /// Create a new sphere collider instance with a given radius at the specified position.
    SphereCollider(const Util::Math::Vector3<float> &position, float radius);

    /// Check if this sphere collider is colliding with another sphere collider.
    /// This is done by calculating the distance between the two sphere centers,
    /// and comparing it to the sum of their radii.
    bool isColliding(const SphereCollider &other) const;

    /// Get the radius of the sphere collider.
    float getRadius() const;

private:

    float radius = 0;
};

}

#endif
