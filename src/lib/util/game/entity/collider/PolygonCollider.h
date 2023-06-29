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
 */

#ifndef HHUOS_POLYGONCOLLIDER_H
#define HHUOS_POLYGONCOLLIDER_H

#include "lib/util/math/Vector2D.h"
#include "lib/util/game/Polygon.h"
#include "lib/util/collection/Pair.h"

namespace Util::Game {

struct Collision {
    double overlap;
    Math::Vector2D axis;
};

class PolygonCollider : public Collider {

public:
    /**
     * Constructor.
     */
    PolygonCollider(Polygon &polygon, Collider::Type colliderType = Collider::DYNAMIC);

    /**
     * Copy Constructor.
     */
    PolygonCollider(const PolygonCollider &other) = delete;

    /**
     * Assignment operator.
     */
    PolygonCollider &operator=(const PolygonCollider &other) = delete;

    /**
     * Destructor.
     */
    ~PolygonCollider() = default;

    Collision isColliding(PolygonCollider &other);

    Polygon &getPolygon();

private:

    static Pair<double, double> projectPolygonOnAxis(Util::Array<Math::Vector2D> vertices, const Math::Vector2D &axis);

    static Math::Vector2D getAxes(Util::Array<Math::Vector2D> vertices, uint32_t index);

    static double getOverlap(Util::Pair<double, double> range, Util::Pair<double, double> rangeOther);

    Polygon &polygon;
};

}

#endif
