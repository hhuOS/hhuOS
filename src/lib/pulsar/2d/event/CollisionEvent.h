/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_PULSAR_2D_COLLISIONEVENT_H
#define HHUOS_LIB_PULSAR_2D_COLLISIONEVENT_H

#include "pulsar/Event.h"
#include "pulsar/2d/Entity.h"
#include "pulsar/2d/collider/RectangleCollider.h"

namespace Pulsar {
namespace D2 {

/// A collision event for 2D rectangle colliders.
/// This event is triggered when two entities with rectangle colliders collide.
/// It contains a reference to the other entity involved in the collision,
/// as well as the side of the `RectangleCollider` that was hit.
/// Collision events are not cancelable. They are created automatically by `D2::Scene`
/// and propagated to the involved entities via `D2::Entity::handleCollisionEvent()`.
class CollisionEvent final : public Event {

public:
    /// Create a new collision event instance.
    /// Collision events are created automatically by the scene when two rectangle colliders collide.
    /// Thus, this constructor is intended for internal use only.
    CollisionEvent(Entity &other, const RectangleCollider::Side side) : other(other), side(side) {}

    /// Get the other entity involved in the collision.
    Entity& getCollidedWidth() const {
        return other;
    }

    /// Get the side of the rectangle collider that was hit during the collision.
    RectangleCollider::Side getSide() const {
        return side;
    }

private:

    Entity &other;
    const RectangleCollider::Side side;
};

}
}

#endif
