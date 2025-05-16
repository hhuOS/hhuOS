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

#include "Math.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

namespace Util::Math {

template<class T>
Vector3<T>::Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

template<class T>
Vector3<T> Vector3<T>::operator*(T value) const {
    return {x * value, y * value, z * value};
}

template<class T>
Vector3<T> Vector3<T>::operator/(T value) const {
    return {x / value, y / value, z / value};
}

template<class T>
Vector3<T> Vector3<T>::operator%(T value) const {
    auto nx = x;
    auto ny = y;
    auto nz = z;

    if (x < 0 || x >= value) {
        auto a = x / value;
        nx = value * (a - (int) a);
        if (nx < 0) nx = value + nx;
    }
    if (y < 0 || y >= value) {
        auto b = y / value;
        ny = value * (b - (int) b);
        if (ny < 0) ny = value + ny;
    }
    if (z < 0 || z >= value) {
        auto c = z / value;
        nz = value * (c - (int) c);
        if (nz < 0) nz = value + nz;
    }

    return {nx, ny, nz};

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
Vector3<T> Vector3<T>::scale(const Vector3 &other) const {
    return {x * other.x, y * other.y, z * other.z};
}

template<class T>
T Vector3<T>::distance(const Vector3 &other) const {
    T pX = pow((other.getX() - getX()), 2);
    T pY = pow((other.getY() - getY()), 2);
    T pZ = pow((other.getZ() - getZ()), 2);

    return sqrt(pX + pY + pZ);
}

// Based on https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
template<class T>
Vector3<T> Vector3<T>::rotate(const Vector3 &rotation) const {
    // Convert deg to rad
    auto convert = 3.1415 / 180;

    T A = rotation.getX() * convert;
    T B = rotation.getY() * convert;
    T C = rotation.getZ() * convert;

    T sinA = Math::sine(A);
    T cosA = Math::cosine(A);
    T sinB = Math::sine(B);
    T cosB = Math::cosine(B);
    T sinC = Math::sine(C);
    T cosC = Math::cosine(C);


    Matrix3x3 rotationMatrix = {
            cosB * cosC, sinA * sinB * cosC - cosA * sinC, cosA * sinB * cosC + sinA * sinC,
            cosB * sinC, sinA * sinB * sinC + cosA * cosC, cosA * sinB * sinC - sinA * cosC,
            -sinB, sinA * cosB, cosA * cosB
    };

    return rotationMatrix * *this;
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
Vector3<T> Vector3<T>::cross(const Vector3 &other) const {
    return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

template<class T>
T Vector3<T>::dot(const Vector3 &other) const {
    return x * other.x + y * other.y + z * other.z;
}

template<class T>
T Vector3<T>::length() const {
    return sqrt(x * x + y * y + z * z);
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
Vector3<T>::Matrix3x3::Matrix3x3(T x1, T y1, T z1, T x2, T y2, T z2, T x3, T y3, T z3) : d11(x1), d12(y1), d13(z1), d21(x2), d22(y2), d23(z2), d31(x3), d32(y3), d33(z3) {}

template<typename T>
typename Vector3<T>::Matrix3x3 Vector3<T>::Matrix3x3::operator*(T value) const {
    return {
        d11 * value, d12 * value, d13 * value,
        d21 * value, d22 * value, d23 * value,
        d31 * value, d32 * value, d33 * value
    };
}

template<typename T>
typename Vector3<T>::Matrix3x3 Vector3<T>::Matrix3x3::operator*(const Matrix3x3 &b) const {
    Matrix3x3 a = *this;
    return {
        a.d11 * b.d11 + a.d12 * b.d21 + a.d13 * b.d31, a.d11 * b.d12 + a.d12 * b.d22 + a.d13 * b.d32, a.d11 * b.d13 + a.d12 * b.d23 + a.d13 * b.d33,
        a.d21 * b.d11 + a.d22 * b.d21 + a.d23 * b.d31, a.d21 * b.d12 + a.d22 * b.d22 + a.d23 * b.d32, a.d21 * b.d13 + a.d22 * b.d23 + a.d23 * b.d33,
        a.d31 * b.d11 + a.d32 * b.d21 + a.d33 * b.d31, a.d31 * b.d12 + a.d32 * b.d22 + a.d33 * b.d32, a.d31 * b.d13 + a.d32 * b.d23 + a.d33 * b.d33
    };
}

template<typename T>
Vector3<T> Vector3<T>::Matrix3x3::operator*(const Vector3 &v) const {
    Matrix3x3 a = *this;
    return {
        a.d11 * v.getX() + a.d12 * v.getY() + a.d13 * v.getZ(),
        a.d21 * v.getX() + a.d22 * v.getY() + a.d23 * v.getZ(),
        a.d31 * v.getX() + a.d32 * v.getY() + a.d33 * v.getZ()
    };
}

}