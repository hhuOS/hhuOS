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

#include "LineDrawer.h"

#include "lib/util/math/Math.h"
#include "lib/util/graphic/PixelDrawer.h"

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

LineDrawer::LineDrawer(const PixelDrawer &pixelDrawer) : pixelDrawer(pixelDrawer) {}

void LineDrawer::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color &color) const {
    // Calculate our deltas
    auto dx = static_cast<int32_t>(x2 - x1);
    auto dy = static_cast<int32_t>(y2 - y1);

    // If the x-axis is the major axis
    if (Math::Math::absolute(dx) >= Math::Math::absolute(dy)) {
        // If x2 < x1, flip the points to have fewer special cases
        if (dx < 0) {
            dx *= -1;
            dy *= -1;
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        // Determine special cases
        if (dy > 0)
            drawLineMajorAxis(x1, y1, 1, 1, dx, dy, true, color);
        else if (dy < 0)
            drawLineMajorAxis(x1, y1, 1, -1, dx, -dy, true, color);
        else
            drawLineSingleAxis(x1, y1, 1, dx, true, color);
    }
        // else the y-axis is the major axis
    else {
        // if y2 < y1, flip the points to have fewer special cases
        if (dy < 0) {
            dx *= -1;
            dy *= -1;
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        // determine special cases
        if (dx > 0)
            drawLineMajorAxis(x1, y1, 1, 1, dy, dx, false, color);
        else if (dx < 0)
            drawLineMajorAxis(x1, y1, -1, 1, dy, -dx, false, color);
        else
            drawLineSingleAxis(x1, y1, 1, dy, false, color);
    }
}

void LineDrawer::drawLineMajorAxis(int32_t x, int32_t y, int8_t xMovement, int8_t yMovement, int32_t dx, int32_t dy, bool majorAxisX, const Color &color) const {
    // Calculate some constants
    const int32_t dx2 = dx * 2;
    const int32_t dy2 = dy * 2;
    const int32_t diffDy2Dx2 = dy2 - dx2;

    // Calculate the starting error value
    auto error = dy2 - dx;

    // Draw the first pixel
    pixelDrawer.drawPixel(x, y, color);

    // lLop across the major axis
    while (dx-- > 0) {
        if (error > 0) {
            // Move on major axis and minor axis
            x += xMovement;
            y += yMovement;
            error += diffDy2Dx2;
        } else {
            // Move on major axis only
            if (majorAxisX) {
                x += xMovement;
            } else {
                y += yMovement;
            }

            error += dy2;
        }

        // draw the next pixel
        pixelDrawer.drawPixel(x, y, color);
    }
}

void LineDrawer::drawLineSingleAxis(int32_t x, int32_t y, int8_t movement, int32_t dx, bool majorAxisX, const Color &color) const {
    // Draw the first pixel
    pixelDrawer.drawPixel(x, y, color);

    // loop across the major axis and draw the rest of the pixels
    while (dx-- > 0) {
        if (majorAxisX) {
            x += movement;
        } else {
            y += movement;
        }

        pixelDrawer.drawPixel(x, y, color);
    };

}

void LineDrawer::swap(int32_t *a, int32_t *b) {
    uint32_t h = *a;
    *a = *b;
    *b = h;
}

}