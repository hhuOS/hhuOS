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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_ROGUE3D_ROGUEPLAYER_H
#define HHUOS_ROGUE3D_ROGUEPLAYER_H

#include <stdint.h>

#include "lib/util/game/3d/Entity.h"

class Player : public Util::Game::D3::Entity {

public:
    /**
     * Constructor.
     */
    explicit Player();

    Player(uint32_t damage, uint32_t health, uint32_t level);

    /**
     * Copy Constructor.
     */
    Player(const Player &other)=delete;

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

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    void takeDamage(uint8_t damage);

    bool shoot();

    [[nodiscard]] uint32_t getHealth() const;

    [[nodiscard]] uint32_t getDamage() const;

    [[nodiscard]] uint32_t getLevel() const;

    void healthUp();

    void dmgUp();

    static const constexpr uint32_t TAG = 1;

private:

    uint32_t projectileMax = 5;
    uint32_t projectileCounter = 0;
    double shootTimer = 0;

    int32_t health = 3;
    int32_t maxHealth = 10;

    uint32_t damage = 1;
    uint32_t maxDmg = 10;

    uint32_t level = 1;
    double invulnerabilityTime = 0;

    static uint32_t DRAW_LIST_ID;
};

#endif