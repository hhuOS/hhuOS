/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_ENEMY_H
#define HHUOS_ENEMY_H

#include <stdint.h>

#include "lib/util/game/3d/Model.h"
#include "lib/util/math/Random.h"

class Player;

namespace Util {
template <typename T> class ArrayList;
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
}  // namespace Util

class Enemy : public Util::Game::D3::Model {

public:

    enum Type {
        STATIONARY = 1,
        ORBIT_PLAYER_CLOCKWISE = 2,
        ORBIT_PLAYER_COUNTER_CLOCKWISE = 3,
        FLY_TOWARDS_PLAYER = 4,
        KEEP_DISTANCE = 5
    };

    /**
     * Constructor.
     */
    Enemy(Player &player, Util::ArrayList<Enemy*> &enemies, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, double scale, Type type);

    /**
     * Copy Constructor.
     */
    Enemy(const Enemy &other) = delete;

    /**
     * Assignment operator.
     */
    Enemy &operator=(const Enemy &other) = delete;

    /**
     * Destructor.
     */
    ~Enemy() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    [[nodiscard]] int16_t getHealth() const;

    void takeDamage(uint8_t damage);

    static const constexpr uint32_t TAG = 3;

private:

    Player &player;
    Util::ArrayList<Enemy*> &enemies;
    double goalScale;
    Type type;

    Util::Math::Random random;
    int16_t health = 50;
    double invulnerabilityTimer = 0;
    double missileTimer = 0;
    double spawnTimer = 0.5;

    static const Util::Math::Vector3<double> MAX_ROTATION_DELTA;
};

#endif
