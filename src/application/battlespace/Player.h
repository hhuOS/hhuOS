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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_APPLICATION_BATTLESPACE_PLAYER_H
#define HHUOS_APPLICATION_BATTLESPACE_PLAYER_H

#include <stddef.h>
#include <stdint.h>

#include <pulsar/3d/Entity.h>
#include <util/math/Vector3.h>

class Enemy;

/// Represents the player.
/// The camera follows the player's position.
/// Its speed and direction can be adjusted via key presses.
/// The player can shoot missiles and gains points if they destroy enemy ships or missiles.
/// The player's position is updated by the scene, not this class.
class Player : public Pulsar::D3::Entity {

public:
    /// Create a new player instance.
    /// The list of enemies is required to draw the HUD map.
    explicit Player(const Util::ArrayList<Enemy*> &enemies);

    /// Update the invulnerability timer and missile cooldown timer with the given delta time.
    void onUpdate(float delta) override;

    /// Draw the HUD.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Handle collisions with other objects.
    /// The player take damage depending on the object type.
    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

    /// Check if the player is currently allowed to shoot a missile.
    /// After shooting a missile, a cooldown time starts, during which the player may not fire further missiles.
    bool mayFireMissile();

    /// Get the current health of the player.
    uint8_t getHealth() const {
        return health;
    }

    /// Decrease the player's health by the given amount of `damage`.
    /// If the health sinks below zero, the game is lost.
    void takeDamage(uint8_t damage);

    /// Add points to the current score.
    void addScore(const size_t points) {
        score += points;
    }

    /// Get the current score that the player has earned.
    size_t getScore() const {
        return score;
    }

    /// Set the movement direction of the player.
    void setMovementDirection(const Util::Math::Vector3<float> &direction) {
        currentMovementDirection = direction;
    }

    /// Get the direction in which player is currently moving as a 3D vector.
    const Util::Math::Vector3<float>& getCurrentMovementDirection() const {
        return currentMovementDirection;
    }

    /// Set the movement speed of the player.
    void setSpeed(float speed) {
        Player::speed = speed;
    }

    /// Get the current movement speed of the player.
    float getSpeed() const {
        return speed;
    }

    /// Unique tag to distinguish the player from other object types in collisions.
    static constexpr size_t TAG = 0;

private:

    const Util::ArrayList<Enemy*> &enemies;

    uint8_t health = 100;
    size_t score = 0;

    float speed = 0.0;
    Util::Math::Vector3<float> currentMovementDirection;

    float invulnerabilityTimer = 0;
    float missileTimer = 0;

    static constexpr float MISSILE_COOLDOWN_TIME = 1.0f;
    static constexpr float INVULNERABILITY_TIME = 0.5f;
};

#endif
