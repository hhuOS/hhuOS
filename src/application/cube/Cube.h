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

#include <cstdint>

#include "lib/util/game/Drawable.h"

namespace Util {
namespace Game {
class Graphics2D;
}  // namespace Game
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

class Cube : public Util::Game::Drawable {

public:
    /**
     * Default Constructor.
     */
    Cube(double x, double y, double size);

    /**
     * Copy Constructor.
     */
    Cube(const Cube &other) = default;

    /**
     * Assignment operator.
     */
    Cube &operator=(const Cube &other) = default;

    /**
     * Destructor.
     */
    ~Cube() override = default;

    void draw(Util::Game::Graphics2D &graphics) const override;

    void rotate(double angleX, double angleY, double angleZ);

private:

    double coordinates[8][3]{};
    double x, y, size;

    // Cube indices
    static const constexpr uint8_t indX = 0, indY = 1, indZ = 2;

    static const Util::Graphic::Color color;
};

#endif
