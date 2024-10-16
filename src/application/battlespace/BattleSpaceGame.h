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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_BATTLESPACEGAME_H
#define HHUOS_BATTLESPACEGAME_H

#include <stdint.h>

#include "lib/util/game/3d/Scene.h"
#include "lib/util/game/KeyListener.h"
#include "lib/util/math/Random.h"
#include "Player.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Vector3D.h"

class Enemy;

namespace Util {
namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class BattleSpaceGame : public Util::Game::D3::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    BattleSpaceGame() = default;

    /**
     * Copy Constructor.
     */
    BattleSpaceGame(const BattleSpaceGame &other) = delete;

    /**
     * Assignment operator.
     */
    BattleSpaceGame &operator=(const BattleSpaceGame &other) = delete;

    /**
     * Destructor.
     */
    ~BattleSpaceGame() override = default;

    void initialize() override;

    void update(double delta) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    Util::Math::Random random;

    Player *player = new Player(enemies);
    Util::ArrayList<Enemy*> enemies;

    Util::Math::Vector3D inputRotation = Util::Math::Vector3D(0, 0, 0);
    Util::Math::Vector3D inputTranslate = Util::Math::Vector3D(0, 0, 0);
    double inputSpeed = 1.0;
    uint16_t difficulty = 0;

    static const constexpr double ENEMY_SPAWN_RANGE = 10.0;
};

#endif
