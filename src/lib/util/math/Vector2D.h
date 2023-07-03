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

#ifndef HHUOS_VECTOR2D_H
#define HHUOS_VECTOR2D_H

namespace Util::Math {

class Vector2D {

public:
    /**
     * Default Constructor.
     */
    Vector2D() = default;

    /**
     * Constructor.
     */
    Vector2D(double x, double y);

    /**
     * Copy Constructor.
     */
    Vector2D(const Vector2D &other) = default;

    /**
     * Assignment operator.
     */
    Vector2D &operator=(const Vector2D &other) = default;

    /**
     * Destructor.
     */
    ~Vector2D() = default;

    Vector2D operator*(double value) const;

    Vector2D operator/(double value) const;

    Vector2D operator+(const Vector2D &other) const;

    Vector2D operator-(const Vector2D &other) const;

    bool operator==(const Vector2D &other) const;

    bool operator!=(const Vector2D &other) const;

    [[nodiscard]] Vector2D normalize() const;

    [[nodiscard]] double dotProduct(const Vector2D &other) const;

    [[nodiscard]] double length() const;

    [[nodiscard]] double getX() const;

    [[nodiscard]] double getY() const;

private:

    double x = 0;
    double y = 0;
};

}

#endif
