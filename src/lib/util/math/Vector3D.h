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

#ifndef HHUOS_VECTOR3D_H
#define HHUOS_VECTOR3D_H

#include "lib/util/base/String.h"

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math
}  // namespace Util

namespace Util::Math {

class Vector3D {

public:
    /**
     * Default Constructor.
     */
    Vector3D() = default;

    /**
     * Constructor.
     */
    Vector3D(double x, double y, double z);

    /**
     * Copy Constructor.
     */
    Vector3D(const Vector3D &other) = default;

    /**
     * Assignment operator.
     */
    Vector3D &operator=(const Vector3D &other) = default;

    /**
     * Destructor.
     */
    ~Vector3D() = default;

    Vector3D operator*(double value) const;

    Vector3D operator/(double value) const;

    Vector3D operator%(double value) const;

    Vector3D operator+(const Vector3D &other) const;

    Vector3D operator+(const Vector2D &other) const;

    Vector3D operator-(const Vector3D &other) const;

    bool operator==(const Vector3D &other) const;

    bool operator!=(const Vector3D &other) const;

    [[nodiscard]] Vector3D scale(const Vector3D &other) const;

    [[nodiscard]] double distance(const Vector3D &other) const;

    [[nodiscard]] Vector3D rotate(const Vector3D &rotation) const;

    [[nodiscard]] Vector3D normalize() const;

    [[nodiscard]] Vector3D cross(const Vector3D &other) const;

    [[nodiscard]] double dot(const Vector3D &other) const;

    [[nodiscard]] double length() const;

    [[nodiscard]] double getX() const;

    [[nodiscard]] double getY() const;

    [[nodiscard]] double getZ() const;

private:

    double x = 0;
    double y = 0;
    double z = 0;
};

struct Matrix3x3 {

    Matrix3x3(
            double x1, double y1, double z1,
            double x2, double y2, double z2,
            double x3, double y3, double z3
    );

    Matrix3x3 operator*(double value) const;

    Matrix3x3 operator*(const Matrix3x3 &other) const;

    Vector3D operator*(const Vector3D &vector) const;

    [[nodiscard]] String toString() const;

    double d11 = 0;
    double d12 = 0;
    double d13 = 0;

    double d21 = 0;
    double d22 = 0;
    double d23 = 0;

    double d31 = 0;
    double d32 = 0;
    double d33 = 0;
};

}

#endif
