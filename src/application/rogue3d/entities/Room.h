/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_ROGUE3D_ROOM_H
#define HHUOS_ROGUE3D_ROOM_H

#include <stdint.h>

#include "lib/pulsar/3d/Entity.h"
#include "lib/util/collection/ArrayList.h"

class Enemy;
class Player;
namespace Util {
namespace Math {
class Random;
template <typename T> class Vector3;
}  // namespace Math
}  // namespace Util

class Room : public Pulsar::D3::Entity {

public:
    enum Type{
        NONE = 0,
        START = 1,
        END = 2,
        ROOM = 3
    };

    /**
     * Constructor.
     */
    Room(const Util::Math::Vector3<float> &position, Type type, uint32_t row, uint32_t column);

    /**
     * Copy Constructor.
     */
    Room(const Room &other) = delete;

    /**
     * Assignment operator.
     */
    Room &operator=(const Room &other) = delete;

    /**
     * Destructor.
     */
    ~Room() override = default;

    void initialize() override;

    void onUpdate(float delta) override;

    void draw(Pulsar::Graphics &graphics) const override;

    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

    void enterRoom();

    void leaveRoom();

    void clearAndRollItems();

    void addEnemiesToScene() const;

    void generateBoss(Player &player);

    uint32_t getRow() const;

    uint32_t getColumn() const;

    bool isCleared() const;

    void generateEnemies(Util::Math::Random &random);

    void generateEnemies(Util::Math::Random &random, Player &player);

    void removeEnemyFromList(Enemy *enemy);

    void enterRightRoom() const;

    Type getType() const;

    bool hasTopRoom() const;

    bool hasDownRoom() const;

    bool hasLeftRoom() const;

    bool hasRightRoom() const;

    Room& getTopRoom() const;

    Room& getDownRoom() const;

    Room& getLeftRoom() const;

    Room& getRightRoom() const;

    void setRightRoom(Room &rightRoom);

    void setLeftRoom(Room &leftRoom);

    void setTopRoom(Room &topRoom);

    void setDownRoom(Room &downRoom);

    static const constexpr uint32_t TAG = 1;

private:
    Type type;

    bool cleared = false;
    bool entered = false;
    bool current = false;

    uint32_t row;
    uint32_t column;

    Util::ArrayList<Enemy*> enemies;

    Room *leftRoom = nullptr;
    Room *rightRoom = nullptr;
    Room *topRoom = nullptr;
    Room *downRoom = nullptr;

    static uint32_t DRAW_LIST_ID;
};

#endif
