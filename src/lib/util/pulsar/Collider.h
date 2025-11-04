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

#ifndef HHUOS_LIB_UTIL_GAME_COLLIDER_H
#define HHUOS_LIB_UTIL_GAME_COLLIDER_H

#include "math/Vector2.h"
#include "math/Vector3.h"

namespace Util::Pulsar {

/// Base class for colliders used in the game engine.
/// Colliders define the physical boundaries of entities for collision detection and response.
/// This class works for both 2D and 3D colliders by utilizing a 3D position vector.
/// Depending on the type of game (2D or 3D), colliders can have different behaviors,
/// defined in the derived classes in the `D2` and `D3` namespaces. Games should always use these derived classes
/// instead of this base class directly.
class Collider {

public:
    /// Different types of colliders.
    enum Type {
        /// A static collider just detects collisions and generates collision events but does not move.
        /// This is typically used for immovable objects like walls or floors.
        STATIC,
        /// A dynamic collider detects collisions and responds to them by moving away from the collision.
        /// The associated entity will receive translation events, that can be canceled to prevent the movement.
        /// This is typically used for movable objects like players or enemies.
        DYNAMIC,
        /// A permeable collider detects collisions and generates collision, similar to a static collider.
        /// However, if a dynamic collider collides with a permeable collider,
        /// the dynamic collider will not move away from the collision.
        /// This is typically used for objects that can be passed through, like collectibles or particles.
        PERMEABLE
    };

    /// Create a new collider instance at the given 2D position with the specified type.
    Collider(const Math::Vector2<double> &position, Type type);

    /// Create a new collider instance at the given 3D position with the specified type.
    Collider(const Math::Vector3<double> &position, Type type);

    /// Set the position of the collider using a 2D vector.
    /// The z-coordinate is set to 0.
    void setPosition(const Math::Vector2<double> &position);

    /// Set the position of the collider using a 3D vector.
    void setPosition(const Math::Vector3<double> &position);

    /// Get the position of the collider as a 3D vector.
    /// For 2D colliders, the z-coordinate is typically 0.
    [[nodiscard]] const Math::Vector3<double>& getPosition() const;

    /// Get the type of the collider.
    [[nodiscard]] Type getType() const;

private:

    Math::Vector3<double> position;
    Type type;
};

}

#endif
