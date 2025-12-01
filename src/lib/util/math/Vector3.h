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

#ifndef HHUOS_LIB_UTIL_MATH_VECTOR3_H
#define HHUOS_LIB_UTIL_MATH_VECTOR3_H

#include "util/math/Vector2.h"

namespace Util::Math {

/// A 3D vector class that supports basic vector operations such as addition, subtraction, scaling, and normalization.
/// It is templated to support float and double types.
/// This class is immutable, meaning that once a Vector is created, its values cannot be changed.
///
/// ## Example
/// ```c++
/// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
/// const auto vector2 = Util::Math::Vector3<float>(4.0f, 5.0f, 6.0f);
///
/// const auto sumVector = vector1 + vector2; // { 5.0f, 7.0f, 9.0f }
/// const auto sumLength = sumVector.length(); // 12.2065556f
/// const auto sumNormalized = sumVector.normalize(); // { 0.408248f, 0.577350f, 0.707107f }
/// ```
template <typename T>
class Vector3 {

public:
    /// Create a new 3D vector with default values (0, 0, 0).
    Vector3() = default;

    /// Create a new 3D vector with the given x, y, and z values.
    Vector3(T x, T y, T z);

    /// Create a new 3D vector by multiplying this vector with a scalar value.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto scaledVector = vector * 2.0f; // { 2.0f, 4.0f, 6.0f }
    /// ```
    Vector3 operator*(T value) const;

    /// Create a new 3D vector by dividing this vector by a scalar value.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector3<float>(2.0f, 4.0f, 6.0f);
    /// const auto dividedVector = vector / 2.0f; // { 1.0f, 2.0f, 3.0f }
    /// ```
    Vector3 operator/(T value) const;

    /// Create a new 3D vector by applying the modulo operation to each component of this vector with a scalar value.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector3<float>(5.0f, 10.0f, 15.0f);
    /// const auto modVector = vector % 6.0f; // { 5.0f, 4.0f, 3.0f }
    ///
    /// const auto negativeVector = Util::Math::Vector3<float>(-5.0f, -10.0f, -15.0f);
    /// const auto modNegativeVector = negativeVector % 6.0f; // { 1.0f, 2.0f, 3.0f }
    /// ```
    Vector3 operator%(T value) const;

    /// Create a new 3D vector from the sum of this vector and another 3D vector.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(4.0f, 5.0f, 6.0f);
    ///
    /// const auto sumVector = vector1 + vector2; // { 5.0f, 7.0f, 9.0f }
    /// ```
    Vector3 operator+(const Vector3 &other) const;

    /// Create a new 3D vector from the sum of this vector and a 2D vector.
    /// The z component is ignored in the 2D vector, and the resulting vector will have the z component of this vector.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector3 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(4.0f, 5.0f);
    ///
    /// const auto sumVector = vector3 + vector2; // { 5.0f, 7.0f, 3.0f }
    /// ```
    Vector3 operator+(const Vector2<T> &other) const;

    /// Create a new 3D vector from the difference of this vector and another 3D vector.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector3<float>(5.0f, 6.0f, 7.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(3.0f, 4.0f, 5.0f);
    ///
    /// const auto diffVector = vector1 - vector2; // { 2.0f, 2.0f, 2.0f }
    /// ```
    Vector3 operator-(const Vector3 &other) const;

    /// Compare this vector with another vector for equality.
    /// ```c++
    /// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector3 = Util::Math::Vector3<float>(4.0f, 5.0f, 6.0f);
    ///
    /// bool areEqual = (vector1 == vector2); // true
    /// areEqual = (vector1 == vector3); // false
    /// ```
    bool operator==(const Vector3 &other) const;

    /// Compare this vector with another vector for inequality.
    ///
    /// ### Example
    /// ```c+++
    /// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector3 = Util::Math::Vector3<float>(4.0f, 5.0f, 6.0f);
    ///
    /// bool areNotEqual = (vector1 != vector2); // false
    /// areNotEqual = (vector1 != vector3); // true
    /// ```
    bool operator!=(const Vector3 &other) const;

    /// Calculate the length (magnitude) of this vector.
    /// The length is calculated by taking the square root of the sum of the squares of its components:
    /// ```c++
    /// sqrt(x * x + y * y + z * z)
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector3<float>(3.0f, 4.0f, 5.0f);
    /// const auto length = vector.length(); // 7.071f (sqrt(3.0f^2 + 4.0f^2 + 5.0f^2))
    /// ```
    T length() const;

    /// Normalize this vector to have a length of 1.
    /// This is done by dividing each component by the vector's length.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector3<float>(3.0f, 4.0f, 5.0f); // Length is 7.071f
    /// const auto normalizedVector = vector.normalize(); // { 0.424f, 0.566f, 0.707f }
    /// ```
    Vector3 normalize() const;

    /// Calculate the distance between this vector and another vector.
    /// This is done using the Euclidean distance formula:
    /// ```c++
    /// sqrt(pow(other.x - x, 2) + pow(other.y - y, 2) + pow(other.z - z, 2))
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(4.0f, 6.0f, 8.0f);
    ///
    /// const auto distance = vector1.distance(vector2); // 6.403f (sqrt(3.0f^2 + 4.0f^2 + 5.0f^2))
    /// ```
    T distance(const Vector3 &other) const;

    /// Calculate the dot product of this vector and another vector.
    /// The result is a scalar value calculated as:
    /// ```c++
    /// x * other.x + y * other.y + z * other.z
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(4.0f, 5.0f, 6.0f);
    ///
    /// const auto dotProduct = vector1.dotProduct(vector2); // 32.0f (1.0f * 4.0f + 2.0f * 5.0f + 3.0f * 6.0f)
    /// ```
    T dotProduct(const Vector3 &other) const;

    /// Calculate the cross product of this vector and another vector.
    /// The result is a new vector that is perpendicular to both vectors in 2D space.
    /// It is calculated as:
    /// ```c++
    /// { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x }
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto vector2 = Util::Math::Vector3<float>(4.0f, 5.0f, 6.0f);
    ///
    /// const auto crossProduct = vector1.crossProduct(vector2); // { -3.0f, 6.0f, -3.0f }
    /// ```
    Vector3 crossProduct(const Vector3 &other) const;

    /// Rotate this vector around the x, y, and z axes by the specified angles in degrees.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector3<float>(1.0f, 2.0f, 3.0f);
    /// const auto rotation = Util::Math::Vector3<float>(30.0f, 45.0f, 60.0f);
    ///
    /// // Rotate the vector by 30° around the x-axis, 45° around the y-axis, and 60° around the z-axis
    /// const auto rotatedVector = vector.rotate(rotation); // { 1.425f, 2.932f, 1.837f }
    /// ```
    Vector3 rotate(const Vector3 &rotation) const;

    /// Return the x component of this vector.
    T getX() const;

    /// Return the y component of this vector.
    T getY() const;

    /// Return the z component of this vector.
    T getZ() const;

private:
    /// A 3x3 matrix class used exclusively for rotation operations in Vector3.
    struct Matrix3x3 {
        /// Create a new 3x3 matrix with the given values.
        Matrix3x3(T x1, T y1, T z1, T x2, T y2, T z2, T x3, T y3, T z3);

        /// Multiply this matrix with a 3D vector and return the resulting vector.
        Vector3 operator*(const Vector3 &vector) const;

        T d11, d12, d13;
        T d21, d22, d23;
        T d31, d32, d33;
    };

    T x = 0;
    T y = 0;
    T z = 0;
};

template class Vector3<float>;
template class Vector3<double>;

}

#endif
