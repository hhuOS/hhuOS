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
 */

#ifndef HHUOS_FLEET_H
#define HHUOS_FLEET_H

#include <cstdint>

#include "lib/util/math/Random.h"

class Fleet {

public:
    /**
     * Constructor.
     */
    explicit Fleet(uint32_t size, double initialSpeed);

    /**
     * Copy Constructor.
     */
    Fleet(const Fleet &other) = delete;

    /**
     * Assignment operator.
     */
    Fleet &operator=(const Fleet &other) = delete;

    /**
     * Destructor.
     */
    ~Fleet() = default;

    void changeDirection();

    void increaseVelocity();

    void moveDown();

    void decreaseSize();

    void applyChanges();

    [[nodiscard]] double getVelocity() const;

    [[nodiscard]] bool isMovingDown() const;

    [[nodiscard]] double getRandomNumber();

private:

    uint32_t size;
    double velocity = 1.0;
    double moveDownCounter = 0;
    Util::Math::Random random;

    double nextVelocity = velocity;
};

#endif
