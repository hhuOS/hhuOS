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
 */

#ifndef HHUOS_BUGDEFENDER_H
#define HHUOS_BUGDEFENDER_H

#include "lib/util/game/Scene.h"
#include "lib/util/game/KeyListener.h"
#include "Ship.h"

class BugDefender : public Util::Game::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    BugDefender();

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

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics2D &graphics) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

private:

    Ship *ship = new Ship(Util::Math::Vector2D(-0.1414, -0.8));

    static const constexpr double BACKGROUND_TILE_SIZE = 0.1;
    static const constexpr uint32_t BACKGROUND_TILE_COUNT = 7;
};

#endif
