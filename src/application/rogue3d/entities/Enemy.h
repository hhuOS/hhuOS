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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_ROGUE3D_ROGUEENEMY_H
#define HHUOS_ROGUE3D_ROGUEENEMY_H

#include <stdint.h>

#include "lib/pulsar/3d/Entity.h"
#include "lib/util/math/Vector3.h"

namespace Util::Graphic {
class Color;
}

class Room;
class Player;

class Enemy : public Pulsar::D3::Entity {

public:
    enum Type {
        CHASING = 0,
        ONLY_SHOOTING = 1,
        BOSS = 2
      };

    /**
     * Constructor.
     */
    Enemy(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation, Room &room, Player &curPlayer, float radius);

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

    void draw(Pulsar::Graphics &graphics) const override;

    void setType(Type type);

    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

    void onUpdate(float delta) override;

    void shoot();

    void shoot3();
    
    void setRightVector(const Util::Math::Vector3<float> &newRight);

    void setUpVector(const Util::Math::Vector3<float> &newUp);

    void setColor(const Util::Graphic::Color &color);

    void setColor(const Util::Math::Vector3<float> &newColor);

    void setActive();

    void takeDamage(uint8_t damage);

    static const constexpr uint32_t TAG = 3;

    static const constexpr int32_t ENEMY_INIT_HEALTH = 3;
    static const constexpr int32_t BOSS_INIT_HEALTH = 20;

private:

    Type type = CHASING;
    bool active = false;
    float damage = 1;

    Player &player;
    Room &room;

    int32_t health = ENEMY_INIT_HEALTH;
    int32_t initHealth = ENEMY_INIT_HEALTH;

    float cooldown = 0.9;

    Util::Math::Vector3<float> bossMovement = Util::Math::Vector3<float>(1, 0, 0);

    static uint32_t ENEMY_LIST_ID;
    static uint32_t BOSS_LIST_ID;
};

#endif