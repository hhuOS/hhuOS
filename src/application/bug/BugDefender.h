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
 */

#ifndef HHUOS_BUG_BUGDEFENDER_H
#define HHUOS_BUG_BUGDEFENDER_H

#include <stddef.h>

#include "Ship.h"
#include "Fleet.h"

#include <util/math/Random.h>
#include <util/math/Vector2.h>
#include <pulsar/2d/Scene.h>
#include <pulsar/audio/AudioTrack.h>
#include <pulsar/audio/AudioHandle.h>

/// The main scene of buf defender.
/// It processes user input and controls the player ship accordingly.
/// On initialization, it creates the player ship and enemy bugs.
/// It is also responsible for initializing the background graphics.
/// The background is static but looks a bit different from game to game.
/// It is built from square tiles, with each tile using a random sprite.
class BugDefender : public Pulsar::D2::Scene {

public:
    /// Create a new bug defender game instance.
    /// No entities are created yet. This is done by `initialize()`.
    BugDefender() = default;

    /// Create the player ship enemy bugs and add them to the scene.
    /// Furthermore, the game music is loaded.
    void initialize() override;

    /// Apply updates to the enemy fleet (`EnemyFleet::applyChanges()`).
    void update(float delta) override;

    /// Draw the background graphics.
    /// The sky is made up of square tiles, each one using one of seven random sprites.
    /// The planet surface is made up of rectangular tiles, each one using one of two random sprites.
    /// This way, the background looks different on each playthrough.
    bool initializeBackground(Pulsar::Graphics &graphics) override;

    /// Handle key presses (e.g., by setting the player's velocity when arrow keys are pressed).
    void keyPressed(const Util::Io::KeyEvent &key) override;

    /// Handle key releases (e.g., by stopping the player's movement when arrow keys are released).
    void keyReleased(const Util::Io::KeyEvent &key) override;

private:

    Fleet enemyFleet = Fleet(BUGS_PER_ROW * BUGS_PER_COLUMN, 0.25);
    Ship *ship = new Ship(Util::Math::Vector2<float>(-0.1414, -0.8));

    Pulsar::AudioTrack backgroundMusic;
    Pulsar::AudioHandle backgroundMusicHandle;

    Util::Math::Random random;

    static constexpr size_t BUGS_PER_ROW = 8;
    static constexpr size_t BUGS_PER_COLUMN = 6;

    static constexpr float BACKGROUND_TILE_WIDTH = 0.1;
    static constexpr float BACKGROUND_TILE_HEIGHT = 0.1;
    static constexpr size_t BACKGROUND_TILE_COUNT = 7;
    static constexpr float BACKGROUND_TILES_PER_ROW = 1 / BACKGROUND_TILE_WIDTH + 1;
    static constexpr float BACKGROUND_TILES_PER_COLUMN = 1 / BACKGROUND_TILE_HEIGHT + 1;

    static constexpr float PLANET_TILE_WIDTH = 0.8;
    static constexpr float PLANET_TILE_HEIGHT = 0.2;
    static constexpr size_t PLANET_TILE_COUNT = 2;
    static constexpr float PLANET_TILES_PER_ROW = 1 / PLANET_TILE_WIDTH + 1;
};

#endif
