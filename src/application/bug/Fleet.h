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

#ifndef HHUOS_APPLICATION_BUG_FLEET_H
#define HHUOS_APPLICATION_BUG_FLEET_H

#include <stddef.h>

#include <util/math/Random.h>

/// Organizes all enemy bugs into a fleet.
/// The fleet does not hold references to the bugs, but it contains information on movement speed and direction.
class Fleet {

public:
    /// Create a new fleet instance for the given number of enemy bugs and initial movement speed.
    explicit Fleet(const size_t size, const float initialSpeed) : size(size), velocity(initialSpeed) {}

    /// Change movement direction (left/right) by negating the velocity.
    void changeDirection() {
        nextVelocity = -velocity;
    }

    /// Increase the velocity (movement speed).
    void increaseVelocity() {
        nextVelocity *= 1.25;
    }

    /// Set the move down status variable, which signals all bugs that they should move downward once.
    void moveDown() {
        // Set counter to 2. On the next frame, the engine will first update the scene,
        // which calls `Fleet::applyChanges()`, which decreases the counter.
        // The entities are updated afterward, so that the count is 1 when the enemy bugs call `Fleet::isMovingDown()`.
        moveDownCounter = 2;
    }

    /// Decrease the number of active enemy bugs.
    void decreaseSize() {
        if (size > 0) {
            size--;
        }
    }

    /// Get the current velocity (movement speed) for all enemy bugs.
    float getVelocity() const {
        return nextVelocity;
    }

    /// Check whether enemy bugs should move down.
    bool isMovingDown() const {
        return moveDownCounter == 1;
    }

    /// Get a random number in [0,1).
    /// This way, all enemy bugs use the same random number generator.
    float getRandomNumber() {
        return random.getRandomNumber<float>();
    }

    /// This function is called once per frame by the bug defender scene and updates the move down status and velocity.
    /// If the fleet size has decreased to zero, switch to the game over screen.
    void applyChanges();

private:

    size_t size;

    size_t moveDownCounter = 0;
    float velocity = 1.0;
    float nextVelocity = velocity;

    Util::Math::Random random;
};

#endif
