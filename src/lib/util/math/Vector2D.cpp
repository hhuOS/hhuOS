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

#include "Vector2D.h"
#include "Math.h"

namespace Util::Math {

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}

Vector2D Vector2D::operator*(double value) const {
    return { x * value, y * value };
}

Vector2D Vector2D::operator/(double value) const {
    return { x / value, y / value};
}

Vector2D Vector2D::operator+(const Vector2D &other) const {
    return { x + other.x, y + other.y };
}

Vector2D Vector2D::operator-(const Vector2D &other) const {
    return { x - other.x, y - other.y };
}

bool Vector2D::operator==(const Vector2D &other) const {
    return x == other.x && y == other.y;
}

bool Vector2D::operator!=(const Vector2D &other) const {
    return x != other.x || y != other.y;
}

Vector2D Vector2D::normalize() const {
    auto len = length();
    return len == 0 ? *this : *this / len;
}

double Vector2D::dotProduct(const Vector2D &other) const {
    return (x * other.x) + (y * other.y);
}

double Vector2D::length() const {
    return sqrt(x * x + y * y);
}

double Vector2D::getX() const {
    return x;
}

double Vector2D::getY() const {
    return y;
}

}