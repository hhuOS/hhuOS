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
 */

#include "Vector3.h"

#include "util/math/Math.h"

namespace Util::Math {

template<class T>
Vector3<T>::Vector3(const T x, const T y, const T z) : x(x), y(y), z(z) {}

template<class T>
Vector3<T> Vector3<T>::operator*(const T value) const {
    return {x * value, y * value, z * value};
}

template<class T>
Vector3<T> Vector3<T>::operator/(const T value) const {
    return {x / value, y / value, z / value};
}

template<class T>
Vector3<T> Vector3<T>::operator%(const T value) const {
    return { modulo(x, value), modulo(y, value), modulo(z, value) };
}

template<class T>
Vector3<T> Vector3<T>::operator+(const Vector3 &other) const {
    return {x + other.x, y + other.y, z + other.z};
}

template<class T>
Vector3<T> Vector3<T>::operator+(const Vector2<T> &other) const {
    return {x + other.getX(), y + other.getY(), z};
}

template<class T>
Vector3<T> Vector3<T>::operator-(const Vector3 &other) const {
    return {x - other.x, y - other.y, z - other.z};
}

template<class T>
bool Vector3<T>::operator==(const Vector3 &other) const {
    return x == other.x && y == other.y && z == other.z;
}

template<class T>
bool Vector3<T>::operator!=(const Vector3 &other) const {
    return x != other.x || y != other.y || z != other.z;
}

template<class T>
T Vector3<T>::length() const {
    return sqrt(x * x + y * y + z * z);
}

template<class T>
Vector3<T> Vector3<T>::normalize() const {
    const auto len = length();
    if (len == 0) {
        return *this;
    }

    return *this * (1 / len);
}

template<class T>
T Vector3<T>::distance(const Vector3 &other) const {
    const auto pX = pow(other.getX() - getX(), 2);
    const auto pY = pow(other.getY() - getY(), 2);
    const auto pZ = pow(other.getZ() - getZ(), 2);

    return sqrt(pX + pY + pZ);
}

template<class T>
T Vector3<T>::dotProduct(const Vector3 &other) const {
    return x * other.x + y * other.y + z * other.z;
}

template<class T>
Vector3<T> Vector3<T>::crossProduct(const Vector3 &other) const {
    return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
}

// Based on https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
template<class T>
Vector3<T> Vector3<T>::rotate(const Vector3 &rotation) const {
    const auto a = toRadians(rotation.getX());
    const auto b = toRadians(rotation.getY());
    const auto c = toRadians(rotation.getZ());

    const auto sinA = sine(a);
    const auto cosA = cosine(a);
    const auto sinB = sine(b);
    const auto cosB = cosine(b);
    const auto sinC = sine(c);
    const auto cosC = cosine(c);

    const auto rotationMatrix = Matrix3x3 {
        cosB * cosC, sinA * sinB * cosC - cosA * sinC, cosA * sinB * cosC + sinA * sinC,
        cosB * sinC, sinA * sinB * sinC + cosA * cosC, cosA * sinB * sinC - sinA * cosC,
        -sinB, sinA * cosB, cosA * cosB
    };

    return rotationMatrix * *this;
}

template<class T>
T Vector3<T>::getX() const {
    return x;
}

template<class T>
T Vector3<T>::getY() const {
    return y;
}

template<class T>
T Vector3<T>::getZ() const {
    return z;
}

template<typename T>
Vector3<T>::Matrix3x3::Matrix3x3(
    const T x1, const T y1, const T z1, const T x2, const T y2, const T z2, const T x3, const T y3, const T z3)
    : d11(x1), d12(y1), d13(z1), d21(x2), d22(y2), d23(z2), d31(x3), d32(y3), d33(z3) {}

template<typename T>
Vector3<T> Vector3<T>::Matrix3x3::operator*(const Vector3 &vector) const {
    const Matrix3x3 &a = *this;
    const Vector3 &v = vector;

    return {
        a.d11 * v.getX() + a.d12 * v.getY() + a.d13 * v.getZ(),
        a.d21 * v.getX() + a.d22 * v.getY() + a.d23 * v.getZ(),
        a.d31 * v.getX() + a.d32 * v.getY() + a.d33 * v.getZ()
    };
}

}