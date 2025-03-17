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

#ifndef HHUOS_VECTOR3D_H
#define HHUOS_VECTOR3D_H

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

namespace Util::Math {

template <typename T>
class Vector3 {

public:
    /**
     * Default Constructor.
     */
    Vector3() = default;

    /**
     * Constructor.
     */
    Vector3(T x, T y, T z);

    /**
     * Copy Constructor.
     */
    Vector3(const Vector3 &other) = default;

    /**
     * Assignment operator.
     */
    Vector3 &operator=(const Vector3 &other) = default;

    /**
     * Destructor.
     */
    ~Vector3() = default;

    Vector3 operator*(T value) const;

    Vector3 operator/(T value) const;

    Vector3 operator%(T value) const;

    Vector3 operator+(const Vector3 &other) const;

    Vector3 operator+(const Vector2<T> &other) const;

    Vector3 operator-(const Vector3 &other) const;

    bool operator==(const Vector3 &other) const;

    bool operator!=(const Vector3 &other) const;

    [[nodiscard]] Vector3 scale(const Vector3 &other) const;

    [[nodiscard]] T distance(const Vector3 &other) const;

    [[nodiscard]] Vector3 rotate(const Vector3 &rotation) const;

    [[nodiscard]] Vector3 normalize() const;

    [[nodiscard]] Vector3 cross(const Vector3 &other) const;

    [[nodiscard]] T dot(const Vector3 &other) const;

    [[nodiscard]] T length() const;

    [[nodiscard]] T getX() const;

    [[nodiscard]] T getY() const;

    [[nodiscard]] T getZ() const;

private:

    struct Matrix3x3 {
        Matrix3x3(
                T x1, T y1, T z1,
                T x2, T y2, T z2,
                T x3, T y3, T z3
        );

        Matrix3x3 operator*(T value) const;

        Matrix3x3 operator*(const Matrix3x3 &other) const;

        Vector3 operator*(const Vector3 &vector) const;

        T d11 = 0;
        T d12 = 0;
        T d13 = 0;

        T d21 = 0;
        T d22 = 0;
        T d23 = 0;

        T d31 = 0;
        T d32 = 0;
        T d33 = 0;
    };

    T x = 0;
    T y = 0;
    T z = 0;
};

template class Vector3<float>;
template class Vector3<double>;

}

#endif
