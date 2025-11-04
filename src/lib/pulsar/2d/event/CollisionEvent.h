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

#ifndef HHUOS_COLLISIONEVENT_2D_H
#define HHUOS_COLLISIONEVENT_2D_H

#include "lib/pulsar/Event.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"

namespace Pulsar {
namespace D2 {
class Entity;
}  // namespace D2
}  // namespace Pulsar

namespace Pulsar::D2 {

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

    template<typename T>
    [[nodiscard]] T& getCollidedWidth();

    [[nodiscard]] RectangleCollider::Side getSide() const;

private:

    Entity &other;
    const RectangleCollider::Side side;
};

template<typename T>
T& Pulsar::D2::CollisionEvent::getCollidedWidth() {
    return reinterpret_cast<T&>(other);
}

}

#endif
