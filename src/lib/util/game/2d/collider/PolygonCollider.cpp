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
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "PolygonCollider.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/2d/Polygon.h"

namespace Util::Game::D2 {

PolygonCollider::PolygonCollider(Polygon &polygon, Collider::Type colliderType) : Collider(polygon.getCenter(), colliderType), polygon(polygon) {}

Collision PolygonCollider::isColliding(PolygonCollider &other) {
    Collision smallestOverlap = { INT16_MAX, Math::Vector2D() };

    for (uint32_t i = 0; i < polygon.getVertices().length(); i++) {
        auto axis = getAxes(polygon.getVertices(), i);

        auto range = projectPolygonOnAxis(polygon.getVertices(), axis);
        auto rangeOther = projectPolygonOnAxis(other.polygon.getVertices(), axis);


        if ((range.first - rangeOther.second > 0) || (rangeOther.first - range.second > 0)) {
            return { 0, Math::Vector2D() };
        }

        double overlap = getOverlap(range, rangeOther);
        if (overlap < smallestOverlap.overlap) {
            smallestOverlap = {overlap, axis};
        }
    }

    return smallestOverlap;
}

Polygon &PolygonCollider::getPolygon() {
    return polygon;
}

Pair<double, double> PolygonCollider::projectPolygonOnAxis(Util::Array<Math::Vector2D> vertices, const Math::Vector2D &axis) {
    double pMin = axis.dotProduct(vertices[0]);
    double pMax = INT16_MIN;

    for (auto &vertex: vertices) {
        auto dot = axis.dotProduct(vertex);
        pMin = Util::Math::min(pMin, dot);
        pMax = Util::Math::max(pMax, dot);
    }

    return {pMin, pMax};
}

Math::Vector2D PolygonCollider::getAxes(Util::Array<Math::Vector2D> vertices, uint32_t index) {
    auto point1 = vertices[index];
    auto point2 = index >= vertices.length() - 1 ? vertices[0] : vertices[index + 1];
    auto axis = Math::Vector2D(-(point2.getY() - point1.getY()), point2.getX() - point1.getX());
    return axis.normalize();
}

double PolygonCollider::getOverlap(Util::Pair<double, double> range, Util::Pair<double, double> rangeOther) {
    return Util::Math::min(range.second, rangeOther.second) - Util::Math::max(range.first, rangeOther.first);
}

}