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

#ifndef HHUOS_ENTITY_H
#define HHUOS_ENTITY_H

#include "lib/util/math/Vector2D.h"
#include "lib/util/game/Drawable.h"

namespace Util::Game {

class Entity : public Drawable {

friend class Game;

public:
    /**
     * Default Constructor.
     */
    Entity() = default;

    /**
     * Constructor.
     */
    explicit Entity(const Util::Math::Vector2D &position);

    /**
     * Copy Constructor.
     */
    Entity(const Entity &other) = default;

    /**
     * Assignment operator.
     */
    Entity &operator=(const Entity &other) = default;

    /**
     * Destructor.
     */
    ~Entity() override = default;

    virtual void onUpdate(double delta) = 0;

    void translate(const Util::Math::Vector2D &translation);

    void translateX(double x);

    void translateY(double y);

    void setPosition(const Util::Math::Vector2D &position);

    [[nodiscard]] const Util::Math::Vector2D& getPosition() const;

private:

    void update(double delta);

    Math::Vector2D position{};
};

}

#endif
