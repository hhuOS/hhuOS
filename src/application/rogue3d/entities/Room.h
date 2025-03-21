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

#ifndef HHUOS_ROGUE3D_ROOM_H
#define HHUOS_ROGUE3D_ROOM_H

#include "lib/util/game/3d/Entity.h"
#include "lib/util/math/Vector3.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Random.h"

class Enemy;
class Player;

namespace Util {
namespace Game {
class Graphics;
namespace D3 {
class CollisionEvent;
}  // namespace D3
}  // namespace Game
}  // namespace Util

class Room : public Util::Game::D3::Entity {

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
    Room(const Util::Math::Vector3<double> &position, Type type, uint32_t row, uint32_t column);

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

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    void enterRoom();

    void leaveRoom();

    void clearAndRollItems();

    void addEnemiesToScene() const;

    void generateBoss(Player &player);

    [[nodiscard]] uint32_t getRow() const;

    [[nodiscard]] uint32_t getColumn() const;

    [[nodiscard]] bool isCleared() const;

    void generateEnemies(Util::Math::Random &random);

    void generateEnemies(Util::Math::Random &random, Player &player);

    void removeEnemyFromList(Enemy *enemy);

    void enterRightRoom() const;

    [[nodiscard]] Type getType() const;

    [[nodiscard]] bool hasTopRoom() const;

    [[nodiscard]] bool hasDownRoom() const;

    [[nodiscard]] bool hasLeftRoom() const;

    [[nodiscard]] bool hasRightRoom() const;

    [[nodiscard]] Room& getTopRoom() const;

    [[nodiscard]] Room& getDownRoom() const;

    [[nodiscard]] Room& getLeftRoom() const;

    [[nodiscard]] Room& getRightRoom() const;

    void setRightRoom(Room &rightRoom);

    void setLeftRoom(Room &leftRoom);

    void setTopRoom(Room &topRoom);

    void setDownRoom(Room &downRoom);

    static const constexpr uint32_t TAG = 1;

private:
    enum Type type;

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
};

#endif
