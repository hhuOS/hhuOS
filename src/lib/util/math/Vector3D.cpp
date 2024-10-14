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
 */

#include "Vector3D.h"

#include <stdint.h>

#include "Math.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Math {

Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3D Vector3D::operator*(double value) const {
    return {x * value, y * value, z * value};
}

Vector3D Vector3D::operator/(double value) const {
    return {x / value, y / value, z / value};
}

Vector3D Vector3D::operator%(double value) const {
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

Vector3D Vector3D::operator+(const Vector3D &other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vector3D Vector3D::operator+(const Vector2D &other) const {
    return {x + other.getX(), y + other.getY(), z};
}

Vector3D Vector3D::operator-(const Vector3D &other) const {
    return {x - other.x, y - other.y, z - other.z};
}

bool Vector3D::operator==(const Vector3D &other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3D::operator!=(const Vector3D &other) const {
    return x != other.x || y != other.y || z != other.z;
}

Vector3D Vector3D::scale(const Vector3D &other) const {
    return {x * other.x, y * other.y, z * other.z};
}

double Vector3D::distance(const Vector3D &other) const {
    double pX = pow((other.getX() - getX()), 2);
    double pY = pow((other.getY() - getY()), 2);
    double pZ = pow((other.getZ() - getZ()), 2);

    return sqrt(pX + pY + pZ);
}

// Based on https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
Vector3D Vector3D::rotate(const Vector3D &rotation) const {
    // Convert deg to rad
    auto convert = 3.1415 / 180;

    double A = rotation.getX() * convert;
    double B = rotation.getY() * convert;
    double C = rotation.getZ() * convert;

    double sinA = Util::Math::sine(A);
    double cosA = Util::Math::cosine(A);
    double sinB = Util::Math::sine(B);
    double cosB = Util::Math::cosine(B);
    double sinC = Util::Math::sine(C);
    double cosC = Util::Math::cosine(C);


    Matrix3x3 rotationMatrix = {
            cosB * cosC, sinA * sinB * cosC - cosA * sinC, cosA * sinB * cosC + sinA * sinC,
            cosB * sinC, sinA * sinB * sinC + cosA * cosC, cosA * sinB * sinC - sinA * cosC,
            -sinB, sinA * cosB, cosA * cosB
    };

    return rotationMatrix * *this;

}

Vector3D Vector3D::normalize() const {
    return *this * (1 / length());
}

Vector3D Vector3D::cross(const Vector3D &other) const {
    return Vector3D(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

double Vector3D::dot(const Vector3D &other) const {
    return x * other.x + y * other.y + z * other.z;
}

double Vector3D::length() const {
    return sqrt(x * x + y * y + z * z);
}

double Vector3D::getX() const {
    return x;
}

double Vector3D::getY() const {
    return y;
}

double Vector3D::getZ() const {
    return z;
}

Matrix3x3::Matrix3x3(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
        : d11(x1), d12(y1), d13(z1), d21(x2), d22(y2), d23(z2), d31(x3), d32(y3), d33(z3) {}

Matrix3x3 Matrix3x3::operator*(double value) const {
    return {
            d11 * value, d12 * value, d13 * value,
            d21 * value, d22 * value, d23 * value,
            d31 * value, d32 * value, d33 * value
    };

}

Vector3D Matrix3x3::operator*(const Vector3D &v) const {
    Matrix3x3 a = *this;
    return {
            a.d11 * v.getX() + a.d12 * v.getY() + a.d13 * v.getZ(),
            a.d21 * v.getX() + a.d22 * v.getY() + a.d23 * v.getZ(),
            a.d31 * v.getX() + a.d32 * v.getY() + a.d33 * v.getZ()
    };
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &b) const {
    Matrix3x3 a = *this;
    return {
            a.d11 * b.d11 + a.d12 * b.d21 + a.d13 * b.d31, a.d11 * b.d12 + a.d12 * b.d22 + a.d13 * b.d32, a.d11 * b.d13 + a.d12 * b.d23 + a.d13 * b.d33,
            a.d21 * b.d11 + a.d22 * b.d21 + a.d23 * b.d31, a.d21 * b.d12 + a.d22 * b.d22 + a.d23 * b.d32, a.d21 * b.d13 + a.d22 * b.d23 + a.d23 * b.d33,
            a.d31 * b.d11 + a.d32 * b.d21 + a.d33 * b.d31, a.d31 * b.d12 + a.d32 * b.d22 + a.d33 * b.d32, a.d31 * b.d13 + a.d32 * b.d23 + a.d33 * b.d33
    };
}

Util::String Matrix3x3::toString() const {
    return Util::String::format("[%d, %d, %d]\n[%d, %d, %d]\n[%d, %d, %d]",
                                (int32_t) (d11 * 1000.0), (int32_t) (d12 * 1000.0), (int32_t) (d13 * 1000.0),
                                (int32_t) (d21 * 1000.0), (int32_t) (d22 * 1000.0), (int32_t) (d23 * 1000.0),
                                (int32_t) (d31 * 1000.0), (int32_t) (d32 * 1000.0), (int32_t) (d33 * 1000.0));

}

}