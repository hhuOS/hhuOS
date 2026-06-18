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

#ifndef HHUOS_APPLICATION_BATTLESPACE_BATTLESPACEGAME_H
#define HHUOS_APPLICATION_BATTLESPACE_BATTLESPACEGAME_H

#include <stddef.h>

#include "Player.h"

#include <pulsar/3d/Scene.h>
#include <util/math/Random.h>
#include <util/collection/ArrayList.h>
#include <util/math/Vector3.h>

class Enemy;

/// The main scene of battle space.
/// It processes user input and controls the player object accordingly.
/// On initialization, it creates all game objects (player, enemies, astronomicals).
class BattleSpaceGame : public Pulsar::D3::Scene {

public:
    /// Create a new battle space game instance.
    /// No entities are created yet. This is done by `initialize()`.
    BattleSpaceGame() = default;

    /// Create the player and astronomical objects and add them to the scene.
    void initialize() override;

    /// Update the player position and speed according to the given time delta and the last user input.
    /// If there are currently no enemies in the scene, new enemy ships are spawned.
    void update(float delta) override;

    /// Handle key presses (e.g., by manipulating the player's direction when arrow keys are pressed).
    void keyPressed(const Util::Io::KeyEvent &key) override;

    /// Handle key releases.
    void keyReleased(const Util::Io::KeyEvent &key) override;

private:

    Util::Math::Random random;

    Player *player = new Player(enemies);
    Util::ArrayList<Enemy*> enemies;

    Util::Math::Vector3<float> inputRotation = Util::Math::Vector3<float>(0, 0, 0);
    Util::Math::Vector3<float> inputTranslation = Util::Math::Vector3<float>(0, 0, 0);
    float inputSpeed = 1.0;
    size_t difficulty = 0;

    static constexpr float ENEMY_SPAWN_RANGE = 10.0;
    static constexpr size_t PLAYER_MAX_PITCH = 82;
};

#endif
