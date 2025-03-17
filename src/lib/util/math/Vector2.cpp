/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Vector2.h"
#include "Math.h"

namespace Util::Math {

template <typename T>
Vector2<T>::Vector2(T x, T y) : x(x), y(y) {}

template <typename T>
Vector2<T> Vector2<T>::operator*(T value) const {
    return { x * value, y * value };
}

template <typename T>
Vector2<T> Vector2<T>::operator/(T value) const {
    return { x / value, y / value};
}

template <typename T>
Vector2<T> Vector2<T>::operator+(const Vector2 &other) const {
    return { x + other.x, y + other.y };
}

template <typename T>
Vector2<T> Vector2<T>::operator-(const Vector2 &other) const {
    return { x - other.x, y - other.y };
}

template <typename T>
bool Vector2<T>::operator==(const Vector2 &other) const {
    return x == other.x && y == other.y;
}

template <typename T>
bool Vector2<T>::operator!=(const Vector2 &other) const {
    return x != other.x || y != other.y;
}

template <typename T>
Vector2<T> Vector2<T>::normalize() const {
    auto len = length();
    return len == 0 ? *this : *this / len;
}

template <typename T>
T Vector2<T>::dotProduct(const Vector2 &other) const {
    return (x * other.x) + (y * other.y);
}

template <typename T>
T Vector2<T>::length() const {
    return sqrt(x * x + y * y);
}

template <typename T>
T Vector2<T>::getX() const {
    return x;
}

template <typename T>
T Vector2<T>::getY() const {
    return y;
}

}