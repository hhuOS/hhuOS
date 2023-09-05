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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/risch114/bachelorarbeit
 */

#ifndef HHUOS_PLAYER_H
#define HHUOS_PLAYER_H

#include <cstdint>

#include "lib/util/game/3d/Entity.h"
#include "lib/util/math/Vector3D.h"

class Enemy;
namespace Util {
namespace Game {
class Graphics;
namespace D3 {
class CollisionEvent;
}  // namespace D3
}  // namespace Game
template <typename T> class ArrayList;
}  // namespace Util

class Player : public Util::Game::D3::Entity {

public:
    /**
     * Constructor.
     */
    explicit Player(const Util::ArrayList<Enemy*> &enemies);

    /**
     * Copy Constructor.
     */
    Player(const Player &other) = delete;

    /**
     * Assignment operator.
     */
    Player &operator=(const Player &other) = delete;

    /**
     * Destructor.
     */
    ~Player() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void onTransformChange() override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    bool mayFireMissile();

    [[nodiscard]] int16_t getHealth() const;

    void takeDamage(uint8_t damage);

    void addScore(uint32_t points);

    [[nodiscard]] uint32_t getScore() const;

    Util::Math::Vector3D getCurrentMovementDirection();

    void setMovementDirection(Util::Math::Vector3D direction);

    void setSpeedDisplay(double speed);

    static const constexpr uint32_t TAG = 0;

private:

    const Util::ArrayList<Enemy*> &enemies;

    int16_t health = 100;
    uint32_t score = 0;
    double speedDisplay = 0.0;
    double invulnerabilityTimer = 0;
    double missileTimer = 0;
    Util::Math::Vector3D currentMovementDirection;
};

#endif
