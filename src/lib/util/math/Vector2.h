/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_MATH_VECTOR2_H
#define HHUOS_LIB_UTIL_MATH_VECTOR2_H

#include "util/math/Math.h"

namespace Util {
namespace Math {

/// A 2D vector class that supports basic vector operations such as addition, subtraction or normalization.
/// It is templated to support float and double types.
/// This class is immutable, meaning that once a Vector is created, its values cannot be changed.
///
/// ## Example
/// ```c++
/// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
/// const auto vector2 = Util::Math::Vector2<float>(2.0f, 2.0f);
///
/// const auto sumVector = vector1 + vector2; // { 3.0f, 4.0f }
/// const auto sumLength = sumVector.length(); // 5.0f
/// const auto sumNormalized = sumVector.normalize(); // { 0.6f, 0.8f }
/// ```
template <typename T>
class Vector2 {

public:
    /// Create a new 2D vector with default values (0, 0).
    Vector2() = default;

    /// Create a new 2D vector with the given x and y values.
    Vector2(T x, T y) : x(x), y(y) {}

    /// Create a new 2D vector by multiplying this vector with a scalar value.
    /// The resulting vector is calculated as:
    /// ```c++
    /// { x * value, y * value }
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto scaledVector = vector * 2.0f; // { 2.0f, 4.0f }
    /// ```
    Vector2 operator*(const T value) const {
        return { x * value, y * value };
    }

    /// Create a new 2D vector by dividing this vector by a scalar value.
    /// The resulting vector is calculated as:
    /// ```c++
    /// { x / value, y / value }
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector2<float>(4.0f, 8.0f);
    /// const auto dividedVector = vector / 2.0f; // { 2.0f, 4.0f }
    /// ```
    Vector2 operator/(const T value) const {
        return { x / value, y / value};
    }

    /// Create a new 2D vector by applying the modulo operation to each component of this vector with a scalar value.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector2<float>(5.0f, 10.0f);
    /// const auto modVector = vector % 6.0f; // { 5.0f, 4.0f }
    ///
    /// const auto negativeVector = Util::Math::Vector2<float>(-5.0f, -10.0f);
    /// const auto modNegativeVector = negativeVector % 6.0f; // { 1.0f, 2.0f }
    /// ```
    Vector2 operator%(const T value) const {
        return { modulo(x, value), modulo(y, value) };
    }

    /// Create a new 2D vector from the sum of this vector and another vector.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(3.0f, 4.0f);
    ///
    /// const auto sumVector = vector1 + vector2; // { 4.0f, 6.0f }
    /// ```
    Vector2 operator+(const Vector2 &other) const {
        return { x + other.x, y + other.y };
    }

    /// Create a new 2D vector from the difference of this vector and another vector.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(5.0f, 6.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(3.0f, 4.0f);
    ///
    /// const auto diffVector = vector1 - vector2; // { 2.0f, 2.0f }
    /// ```
    Vector2 operator-(const Vector2 &other) const {
        return { x - other.x, y - other.y };
    }

    /// Compare this vector with another vector for equality.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector3 = Util::Math::Vector2<float>(3.0f, 4.0f);
    ///
    /// bool areEqual = (vector1 == vector2); // true
    /// areEqual = (vector1 == vector3); // false
    /// ```
    bool operator==(const Vector2 &other) const {
        return x == other.x && y == other.y;
    }

    /// Compare this vector with another vector for inequality.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector3 = Util::Math::Vector2<float>(3.0f, 4.0f);
    ///
    /// bool areNotEqual = (vector1 != vector2); // false
    /// areNotEqual = (vector1 != vector3); // true
    /// ```
    bool operator!=(const Vector2 &other) const {
        return x != other.x || y != other.y;
    }

    /// Calculate the length (magnitude) of this vector.
    /// The length is calculated by taking the square root of the sum of the squares of its components:
    /// ```c++
    /// sqrt(x * x + y * y)
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector2<float>(3.0f, 4.0f);
    /// const auto length = vector.length(); // 5.0f (3.0f^2 + 4.0f^2 = 25, sqrt(25) = 5)
    /// ```
    T length() const {
        return sqrt(x * x + y * y);
    }

    /// Normalize this vector to have a length of 1.
    /// This is done by dividing each component by the vector's length.
    ///
    /// ### Example
    /// ```c++
    /// const auto vector = Util::Math::Vector2<float>(3.0f, 4.0f); // Length is 5.0f
    /// const auto normalizedVector = vector.normalize(); // { 0.6f, 0.8f }
    /// ```
    Vector2 normalize() const {
        const auto len = length();
        return len == 0 ? *this : *this / len;
    }

    /// Calculate the distance between this vector and another vector.
    /// This is done using the Euclidean distance formula:
    /// ```c++
    /// sqrt(pow(other.x - x, 2) + pow(other.y - y, 2))
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(4.0f, 6.0f);
    ///
    /// const auto distance = vector1.distance(vector2); // 5.0f (sqrt((4.0f - 1.0f)^2 + (6.0f - 2.0f)^2))
    /// ```
    T distance(const Vector2 &other) const {
        const auto pX = pow(other.getX() - getX(), 2);
        const auto pY = pow(other.getY() - getY(), 2);

        return sqrt(pX + pY);
    }

    /// Calculate the dot product of this vector and another vector.
    /// The result is a scalar value calculated as:
    /// ```c++
    /// x * other.x + y * other.y
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(3.0f, 4.0f);
    ///
    /// const auto dotProduct = vector1.dotProduct(vector2); // 11.0f (1.0f * 3.0f + 2.0f * 4.0f)
    /// ```
    T dotProduct(const Vector2 &other) const {
        return x * other.x + y * other.y;
    }

    /// Calculate the cross product of this vector and another vector.
    /// The result is a new vector that is perpendicular to both vectors in 2D space.
    /// It is calculated as:
    /// ```c++
    /// { x * other.y - y * other.x, y * other.x - x * other.y }
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto vector1 = Util::Math::Vector2<float>(1.0f, 2.0f);
    /// const auto vector2 = Util::Math::Vector2<float>(3.0f, 4.0f);
    ///
    /// const auto crossProduct = vector1.cross(vector2); // { -2.0f, 2.0f }
    /// ```
    Vector2 crossProduct(const Vector2 &other) const {
        return { x * other.y - y * other.x, y * other.x - x * other.y };
    }

    /// Return the x component of this vector.
    T getX() const {
        return x;
    }

    /// Return the y component of this vector.
    T getY() const {
        return y;
    }

private:

    T x = 0;
    T y = 0;
};

template class Vector2<float>;
template class Vector2<double>;

}
}

#endif
