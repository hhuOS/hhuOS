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

#ifndef HHUOS_CUBE_H
#define HHUOS_CUBE_H

#include "lib/util/game/Drawable.h"
#include "lib/util/graphic/LineDrawer.h"

class Cube : public Util::Game::Drawable {

public:
    /**
     * Default Constructor.
     */
    Cube();

    /**
     * Copy Constructor.
     */
    Cube(const Cube &other) = delete;

    /**
     * Assignment operator.
     */
    Cube &operator=(const Cube &other) = delete;

    /**
     * Destructor.
     */
    ~Cube() = default;

    void draw(const Util::Graphic::LinearFrameBuffer &lfb) const override;

    void rotate(double angleX, double angleY, double angleZ);

private:

    double coordinates[8][4]{};

    // Cube indices
    static const constexpr uint8_t x = 1, y = 2, z = 3;

    static const Util::Graphic::Color color;
};

#endif
