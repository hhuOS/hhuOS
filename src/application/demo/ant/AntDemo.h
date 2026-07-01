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

#ifndef HHUOS_APPLICATION_DEMO_ANTDEMO_H
#define HHUOS_APPLICATION_DEMO_ANTDEMO_H

#include <stdint.h>

#include <util/math/Random.h>
#include <util/graphic/Color.h>
#include <util/graphic/Colors.h>
#include <util/graphic/LinearFrameBuffer.h>

/// An implementation of "Langton's ant" moving on the given framebuffer.
/// The ant starts at a random position on the screen and starts moving upward.
/// Every time it hits a black pixel, it turns 90° clockwise
/// and changes the color of the pixel before moving to the next one.
/// When it hits a colored pixel, it turns 90° clockwise
/// and sets the pixel to black before moving to the next one.
/// When the ant comes to a screen boundary, its position is set to the opposite site of the screen.
/// Each time it crosses boundaries like that, the color that black pixels are set to, changes.
void antDemo(const Util::Graphic::LinearFrameBuffer &lfb);

/// Represents "Langton's ant" with a position, movement direction, and color that black pixels are set to.
class Ant {

public:
    /// Create a new ant instance that moves inside the coordinates (0, 0) and (limitX, limitY).
    /// Each time it hits a boundary, its position is set to the opposite site of the coordinate system.
    /// When it crosses boundaries like that, the color that black pixels are set to, changes.
    Ant(uint16_t limitX, uint16_t limitY);

    /// Move the ant one pixel forward.
    /// If it crosses a boundary, the color that black pixels are set to, changes.
    void move();

    /// Turn the ant clockwise by 90°.
    void turnClockwise();

    /// Turn the ant counter-clockwise by 90°.
    void turnCounterClockwise();

    /// Get the x-coordinate of the ant's position
    uint16_t getX() const {
        return x;
    }

    /// Get the y-coordinate of the ant's position
    uint16_t getY() const {
        return y;
    }

    /// Get the color that black pixels should be set to.
    const Util::Graphic::Color& getColor() const {
        return color;
    }

private:

    enum Direction {
        UP = 0,
        RIGHT = 90,
        DOWN = 180,
        LEFT = 270
    };

    Util::Math::Random random;

    Util::Graphic::Color color = Util::Graphic::Colors::WHITE;

    uint16_t limitX, limitY;
    uint16_t x, y;
    Direction direction = UP;
};

#endif
