/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Polygon.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/Graphics2D.h"

namespace Util::Game {

Polygon::Polygon(const Array<Math::Vector2D> &vertices) : vertices(vertices) {
    calculateCenter();
}

void Polygon::onUpdate(double delta) {}

void Polygon::draw(Graphics2D &graphics) const {
    graphics.drawPolygon(vertices);
}

void Polygon::scale(double factor) {
    const auto &position = getPosition();
    for (auto &vertex : vertices) {
        vertex = Math::Vector2D(position.getX() + factor * (vertex.getX() - position.getX()),
                                position.getY() + factor * (vertex.getY() - position.getY()));
    }
}

void Polygon::rotate(double angle) {
    const auto &position = getPosition();
    double sine = Math::sine(angle);
    double cosine = Math::cosine(angle);

    for (auto &vertex : vertices) {
        auto d = vertex - position;
        vertex = Math::Vector2D(d.getX() * cosine - d.getY() * sine + position.getX(),
                                d.getX() * sine + d.getY() * cosine + position.getY());
    }
}

void Polygon::translate(Math::Vector2D translation) {
    for (auto &vertex: vertices) {
        vertex = vertex + translation;
    }

    setPosition(getPosition() + translation);
}

void Polygon::calculateCenter() {
    auto sum = Math::Vector2D();
    for (auto &vertex: vertices) {
        sum = sum + vertex;
    }

    setPosition(sum / vertices.length());
}

}