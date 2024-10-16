/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Polygon.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/Graphics.h"

namespace Util::Game::D2 {

Polygon::Polygon(const Array<Math::Vector2D> &vertices) : vertices(vertices) {
    calculateCenter();
}

const Math::Vector2D &Polygon::getCenter() const {
    return center;
}

const Util::Array<Math::Vector2D>& Polygon::getVertices() const {
    return vertices;
}

void Polygon::draw(Graphics &graphics) {
    graphics.drawPolygon2D(vertices);
}

void Polygon::scale(double factor) {
    for (auto &vertex : vertices) {
        vertex = Math::Vector2D(center.getX() + factor * (vertex.getX() - center.getX()),
                                center.getY() + factor * (vertex.getY() - center.getY()));
    }
}

void Polygon::rotate(double angle) {
    double sine = Math::sine(angle);
    double cosine = Math::cosine(angle);

    for (auto &vertex : vertices) {
        auto d = vertex - center;
        vertex = Math::Vector2D(d.getX() * cosine - d.getY() * sine + center.getX(),
                                d.getX() * sine + d.getY() * cosine + center.getY());
    }
}

void Polygon::translate(Math::Vector2D translation) {
    for (auto &vertex: vertices) {
        vertex = vertex + translation;
    }

    center = center + translation;
}

void Polygon::setPosition(const Math::Vector2D &newPosition) {
    auto translation = newPosition - getTopLeft();
    translate(translation);
}

void Polygon::calculateCenter() {
    auto sum = Math::Vector2D();
    for (const auto &vertex: vertices) {
        sum = sum + vertex;
    }

    center = sum / vertices.length();
}

Math::Vector2D Polygon::getTopLeft() {
    Math::Vector2D topLeft = vertices[0];
    for (auto &vertex: vertices) {
        if (vertex.getX() < topLeft.getX()) {
            topLeft = Math::Vector2D(vertex.getX(), topLeft.getY());
        }

        if (vertex.getY() < topLeft.getY()) {
            topLeft = Math::Vector2D(topLeft.getX(), vertex.getY());
        }
    }

    return topLeft;
}

}