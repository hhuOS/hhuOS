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

#ifndef HHUOS_VECTOR2D_H
#define HHUOS_VECTOR2D_H

namespace Util::Math {

template <typename T>
class Vector2 {

public:
    /**
     * Default Constructor.
     */
    Vector2() = default;

    /**
     * Constructor.
     */
    Vector2(T x, T y);

    /**
     * Copy Constructor.
     */
    Vector2(const Vector2 &other) = default;

    /**
     * Assignment operator.
     */
    Vector2 &operator=(const Vector2 &other) = default;

    /**
     * Destructor.
     */
    ~Vector2() = default;

    Vector2 operator*(T value) const;

    Vector2 operator/(T value) const;

    Vector2 operator+(const Vector2 &other) const;

    Vector2 operator-(const Vector2 &other) const;

    bool operator==(const Vector2 &other) const;

    bool operator!=(const Vector2 &other) const;

    [[nodiscard]] Vector2 normalize() const;

    [[nodiscard]] T dotProduct(const Vector2 &other) const;

    [[nodiscard]] T length() const;

    [[nodiscard]] T getX() const;

    [[nodiscard]] T getY() const;

private:

    T x = 0;
    T y = 0;
};

template class Vector2<float>;
template class Vector2<double>;

}

#endif
