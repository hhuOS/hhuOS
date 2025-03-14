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
 */

#ifndef HHUOS_ANT_H
#define HHUOS_ANT_H

#include <stdint.h>

#include "lib/util/graphic/Color.h"
#include "lib/util/math/Random.h"
#include "lib/util/graphic/Colors.h"

namespace Util {
namespace Graphic {
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

void antDemo(const Util::Graphic::LinearFrameBuffer &lfb, uint32_t sleepInterval);

class Ant {

public:

    enum Direction {
        UP = 0, RIGHT = 90, DOWN = 180, LEFT = 270
    };

    /**
     * Constructor.
     */
    Ant(int16_t limitX, int16_t limitY);

    /**
     * Copy Constructor.
     */
    Ant(const Ant &other) = delete;

    /**
     * Assignment operator.
     */
    Ant &operator=(const Ant &other) = delete;

    /**
     * Destructor.
     */
    ~Ant() = default;

    void move();

    void turnClockWise();

    void turnCounterClockWise();

    [[nodiscard]] int16_t getX() const;

    [[nodiscard]] int16_t getY() const;

    [[nodiscard]] const Util::Graphic::Color &getColor() const;

private:

    int16_t limitX, limitY;
    int16_t x, y;
    Direction direction = UP;
    Util::Graphic::Color color = Util::Graphic::Colors::WHITE;
    Util::Math::Random random = Util::Math::Random();
};

#endif
