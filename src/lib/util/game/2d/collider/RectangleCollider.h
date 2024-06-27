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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_RECTANGLECOLLIDER_H
#define HHUOS_RECTANGLECOLLIDER_H

#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Game::D2 {

class RectangleCollider : public Collider {

public:

    enum Side {
        LEFT, RIGHT, TOP, BOTTOM, NONE
    };

    /**
     * Constructor.
     */
    RectangleCollider(const Math::Vector2D &position, const Math::Vector2D &size, Collider::Type type);

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

    [[nodiscard]] const Math::Vector2D& getSize() const;

    void setWidth(double width);

    void setHeight(double height);

    void setSize(const Math::Vector2D &size);

    [[nodiscard]] Side isColliding(const RectangleCollider &other) const;

private:

    Math::Vector2D size;
};

}

#endif
