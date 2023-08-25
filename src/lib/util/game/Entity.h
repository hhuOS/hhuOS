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

#include <cstdint>

#include "Drawable.h"

namespace Util::Game {

namespace D2 {
class Entity;
}

class Entity : public Drawable {

friend class Util::Game::D2::Entity;

public:
    /**
     * Constructor.
     */
    explicit Entity(uint32_t tag);

    /**
     * Copy Constructor.
     */
    Entity(const Entity &other) = delete;

    /**
     * Assignment operator.
     */
    Entity &operator=(const Entity &other) = delete;

    /**
     * Destructor.
     */
    ~Entity() override = default;

    virtual void initialize() = 0;

    virtual void onUpdate(double delta) = 0;

    [[nodiscard]] uint32_t getTag() const;

private:

    uint32_t tag;
};

}

#endif
