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

#ifndef HHUOS_COLLISIONEVENT_H
#define HHUOS_COLLISIONEVENT_H

#include "Event.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"

namespace Util {
namespace Game {
class Entity;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class CollisionEvent : public Event {

public:
    /**
     * Constructor.
     */
    CollisionEvent(Entity &other, RectangleCollider::Side side);

    /**
     * Copy Constructor.
     */
    CollisionEvent(const CollisionEvent &other) = delete;

    /**
     * Assignment operator.
     */
    CollisionEvent &operator=(const CollisionEvent &other) = delete;

    /**
     * Destructor.
     */
    ~CollisionEvent() = default;

    [[nodiscard]] Entity& getCollidedWidth();

    [[nodiscard]] RectangleCollider::Side getSide() const;

private:

    Entity &other;
    const RectangleCollider::Side side;
};

}

#endif
