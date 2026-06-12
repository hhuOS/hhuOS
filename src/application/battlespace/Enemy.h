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

#ifndef HHUOS_APPLICATION_BATTLESPACE_ENEMY_H
#define HHUOS_APPLICATION_BATTLESPACE_ENEMY_H

#include <stddef.h>
#include <stdint.h>

#include <pulsar/3d/Model.h>
#include <util/math/Random.h>

class Player;

/// Represents an enemy ship in the game.
/// Each enemy has one of the strategies defined in `Strategy` to attack the player.
/// The player gains points by destroying enemy ships.
/// The player can also collide with enemy ships, hurting both.
class Enemy : public Pulsar::D3::Model {

public:
    /// Attack strategies that an enemy ship can follow.
    enum Strategy {
        /// Stationary enemies do not move but only shoot missiles at the player from a fixed position.
        STATIONARY = 0,
        /// Orbit around the player in a clockwise direction, shooting missiles at them.
        ORBIT_PLAYER_CLOCKWISE = 1,
        /// Orbit around the player in a counter-clockwise direction, shooting missiles at them.
        ORBIT_PLAYER_COUNTER_CLOCKWISE = 2,
        /// Move directly towards the player, trying to crash into them.
        FLY_TOWARDS_PLAYER = 3,
        /// Try to keep a distance from the player and shoot missiles at them.
        KEEP_DISTANCE = 4
    };

    /// Create a new enemy instance at a given position and with a given rotation and scale.
    /// It follows the specified `strategy` and requires references to the player and all other active enemies.
    /// The player reference is needed to get the player's current position and to increase its gained points,
    /// once the enemy is destroyed.
    /// The enemy ship removes itself from the list of active enemies once it is destroyed.
    Enemy(Player &player, Util::ArrayList<Enemy*> &enemies, Strategy strategy,
        const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation, float scale);

    /// Initialize the enemy ship, loading the 3D model.
    void initialize() override;

    /// Update the enemy ship according to its strategy and the given delta time.
    void onUpdate(float delta) override;

    /// Handle a collision with another entity.
    /// An enemy ship can collide with a missile, another enemy or the player.
    /// In each case, it directly explodes and the player gains the appropriate points.
    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

    /// Get the current health of the enemy ship.
    int16_t getHealth() const {
        return health;
    }

    /// Decrease the enemy's health by the given amount of `damage`.
    /// If the health sinks to zero, the enemy is destroyed and explodes.
    /// The player gains points in this case.
    void takeDamage(uint8_t damage);

    /// Unique tag to distinguish enemy ships from other object types in collisions.
    static constexpr size_t TAG = 3;

private:

    static Util::Math::Vector3<float> findLookAt(const Util::Math::Vector3<float> &from,
        const Util::Math::Vector3<float> &to);

    Player &player;
    Util::ArrayList<Enemy*> &enemies;
    const float targetScale;
    const Strategy strategy;

    Util::Math::Random random;
    uint8_t health = 50;
    float invulnerabilityTimer = 0;
    float missileTimer = 0;
    float spawnTimer = 0.5;

    static const Util::Math::Vector3<float> MAX_ROTATION_DELTA;

    static constexpr float MIN_MISSILE_COOLDOWN_TIME = 2.0f;
    static constexpr float INVULNERABILITY_TIME = 0.5f;
    static constexpr size_t POINTS = 1000;
};

#endif
