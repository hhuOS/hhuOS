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

#ifndef HHUOS_COLLIDER_H
#define HHUOS_COLLIDER_H

#include "lib/util/math/Vector2D.h"

namespace Util::Game {

class Collider {

friend class Entity;

public:

    enum Type {
        STATIC, DYNAMIC
    };

    /**
     * Constructor.
     */
    Collider(const Math::Vector2D &position, Type type);

    /**
     * Copy Constructor.
     */
    Collider(const Collider &other) = default;

    /**
     * Assignment operator.
     */
    Collider &operator=(const Collider &other) = default;

    /**
     * Destructor.
     */
    ~Collider() = default;

    [[nodiscard]] const Math::Vector2D& getPosition() const;

    [[nodiscard]] Type getType() const;

protected:

    Math::Vector2D lastPosition;

    void setPosition(const Math::Vector2D &position);

private:

    Math::Vector2D position;
    Type type;
};

}

#endif
