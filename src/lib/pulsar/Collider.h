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

#ifndef HHUOS_LIB_PULSAR_COLLIDER_H
#define HHUOS_LIB_PULSAR_COLLIDER_H

#include "util/math/Vector2.h"
#include "util/math/Vector3.h"

namespace Pulsar {

/// Base class for colliders used in the game engine.
/// Colliders define the physical boundaries of entities for collision detection and response.
/// This class works for both 2D and 3D colliders by utilizing a 3D position vector.
/// Depending on the type of game (2D or 3D), colliders can have different behaviors,
/// defined in the derived classes in the `D2` and `D3` namespaces. Games should always use these derived classes
/// instead of this base class directly.
class Collider {

public:

    /// Create a new collider of type NONE at the origin.
    /// This is used for entities without a collider.
    Collider() = default;

    /// Create a new collider instance at the given 2D position with the specified type.
    explicit Collider(const Util::Math::Vector2<double> &position);

    /// Create a new collider instance at the given 3D position with the specified type.
    explicit Collider(const Util::Math::Vector3<double> &position);

    /// Set the position of the collider using a 2D vector.
    /// The z-coordinate is set to 0.
    void setPosition(const Util::Math::Vector2<double> &position);

    /// Set the position of the collider using a 3D vector.
    void setPosition(const Util::Math::Vector3<double> &position);

    /// Get the position of the collider as a 3D vector.
    /// For 2D colliders, the z-coordinate is typically 0.
    [[nodiscard]] const Util::Math::Vector3<double>& getPosition() const;

private:

    Util::Math::Vector3<double> position;
};

}

#endif
