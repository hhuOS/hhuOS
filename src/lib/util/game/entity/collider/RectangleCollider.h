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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#ifndef HHUOS_RECTANGLECOLLIDER_H
#define HHUOS_RECTANGLECOLLIDER_H

#include "Collider.h"

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math
}  // namespace Util

namespace Util::Game {

class RectangleCollider : public Collider {

public:

    enum Side {
        LEFT, RIGHT, TOP, BOTTOM, NONE
    };

    /**
     * Constructor.
     */
    RectangleCollider(const Math::Vector2D &position, Collider::Type type, double width, double height);

    /**
     * Copy Constructor.
     */
    RectangleCollider(const RectangleCollider &other) = default;

    /**
     * Assignment operator.
     */
    RectangleCollider &operator=(const RectangleCollider &other) = default;

    /**
     * Destructor.
     */
    ~RectangleCollider() = default;

    static Side getOpposite(Side side);

    [[nodiscard]] double getWidth() const;

    [[nodiscard]] double getHeight() const;

    void setWidth(double width);

    void setHeight(double height);

    [[nodiscard]] Side isColliding(const RectangleCollider &other) const;

private:

    double width;
    double height;
};

}

#endif
