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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_2D_RECTANGLECOLLIDER_H
#define HHUOS_LIB_PULSAR_2D_RECTANGLECOLLIDER_H

#include "util/math/Vector2.h"
#include "pulsar/Collider.h"

namespace Pulsar::D2 {

/// A rectangular collider for 2D collision detection.
/// Each collider has four sides: `LEFT`, `RIGHT`, `TOP` and `BOTTOM`.
/// Collisions with other rectangle collider can be detected by calling `isColliding()`.
/// If a collision is detected, the side of this collider that is colliding with the other collider is returned.
/// This way, games can use this collider as an axis-aligned bounding box (AABB) for simple collision detection.
class RectangleCollider : public Collider {

public:
    /// The sides of the rectangle collider.
    enum Side {
        /// The left side of the rectangle collider.
        LEFT = 0,
        /// The right side of the rectangle collider.
        RIGHT = 1,
        /// The top side of the rectangle collider.
        TOP = 2,
        /// The bottom side of the rectangle collider.
        BOTTOM = 3,
        /// No collision.
        NONE
    };

    /// Create a new rectangle collider of type `NONE` at the origin with zero size.
    /// This is used for entities without a collider.
    RectangleCollider() = default;

    /// Create a new rectangle collider instance at the given position with the given size and type.
    RectangleCollider(const Util::Math::Vector2<double> &position, double width, double height, Type type);

    /// Get the opposite side of the given side.
    /// For example, the opposite side of `LEFT` is `RIGHT`.
    static Side getOpposite(Side side);

    /// Get the width of the rectangle collider.
    [[nodiscard]] double getWidth() const;

    /// Get the height of the rectangle collider.
    [[nodiscard]] double getHeight() const;

    /// Set the width of the rectangle collider.
    void setWidth(double width);

    /// Set the height of the rectangle collider.
    void setHeight(double height);

    /// Set the width and height of the rectangle collider.
    void setSize(double width, double height);
    
    /// Check if this rectangle collider is colliding with another rectangle collider (i.e., if the rectangles overlap).
    /// If a collision is detected, the side of this collider that is colliding with the other collider is returned.
    /// Otherwise, `NONE` is returned.
    [[nodiscard]] Side isColliding(const RectangleCollider &other) const;

private:

    double width = 0;
    double height = 0;
};

}

#endif
