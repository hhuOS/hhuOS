/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_CUBEDEMO_H
#define HHUOS_CUBEDEMO_H

#include <cstdint>

#include "lib/util/game/Game.h"
#include "Cube.h"
#include "lib/util/game/KeyListener.h"

class CubeDemo : public Util::Game::Game, public Util::Game::KeyListener {

public:
    /**
     * Constructor.
     */
    explicit CubeDemo(uint32_t speed = 10);

    /**
     * Copy Constructor.
     */
    CubeDemo(const CubeDemo &other) = delete;

    /**
     * Assignment operator.
     */
    CubeDemo &operator=(const CubeDemo &other) = delete;

    /**
     * Destructor.
     */
    ~CubeDemo() override = default;

    void update(double delta) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

    static const constexpr uint32_t NUM_CUBES = 4;
    Cube *cubes[NUM_CUBES] = {new Cube(-0.5, 0.5, 0.25), new Cube(0.5, 0.5, 0.25), new Cube(0.5, -0.5, 0.25), new Cube(-0.5, -0.5, 0.25)};

    int32_t speed;

    // Rotation angles
    static const constexpr double ANGLE_X = 0.01;
    static const constexpr double ANGLE_Y = 0.0075;
    static const constexpr double ANGLE_Z = 0.005;
};

#endif
