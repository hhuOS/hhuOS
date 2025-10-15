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
 */

#ifndef HHUOS_BUGDEFENDER_H
#define HHUOS_BUGDEFENDER_H

#include <stdint.h>

#include "lib/util/game/2d/Scene.h"
#include "lib/util/game/KeyListener.h"
#include "Ship.h"
#include "Fleet.h"
#include "lib/util/game/audio/AudioTrack.h"
#include "lib/util/game/audio/AudioHandle.h"
#include "lib/util/math/Random.h"
#include "lib/util/math/Vector2.h"

class BugDefender : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    BugDefender() = default;

    /**
     * Copy Constructor.
     */
    BugDefender(const BugDefender &other) = delete;

    /**
     * Assignment operator.
     */
    BugDefender &operator=(const BugDefender &other) = delete;

    /**
     * Destructor.
     */
    ~BugDefender() override = default;

    void initialize() override;

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

    static const constexpr uint32_t BUGS_PER_ROW = 8;
    static const constexpr uint32_t BUGS_PER_COLUMN = 6;

private:

    Fleet enemyFleet = Fleet(BUGS_PER_ROW * BUGS_PER_COLUMN, 0.25);
    Ship *ship = new Ship(Util::Math::Vector2<double>(-0.1414, -0.8));

    Util::Game::AudioTrack backgroundMusic;
    Util::Game::AudioHandle backgroundMusicHandle;

    Util::Math::Random random;

    static const constexpr double BACKGROUND_TILE_WIDTH = 0.1;
    static const constexpr double BACKGROUND_TILE_HEIGHT = 0.1;
    static const constexpr uint32_t BACKGROUND_TILE_COUNT = 7;

    static const constexpr double PLANET_TILE_WIDTH = 0.8;
    static const constexpr double PLANET_TILE_HEIGHT = 0.2;
    static const constexpr uint32_t PLANET_TILE_COUNT = 2;
};

#endif
