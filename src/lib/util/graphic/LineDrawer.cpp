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

namespace Util::Graphic {

LineDrawer::LineDrawer(PixelDrawer &pixelDrawer) : pixelDrawer(pixelDrawer) {}

void LineDrawer::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const Color &color) {
    // Calculate our deltas
    auto dx = static_cast<int32_t>(x2 - x1);
    auto dy = static_cast<int32_t>(y2 - y1);

    // If the x-axis is the major axis
    if (abs(dx) >= abs(dy)) {
        // If x2 < x1, flip the points to have fewer special cases
        if (dx < 0) {
            dx *= -1;
            dy *= -1;
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        // Get the address of the pixel at (x1,y1)
        uint32_t startPixel = y1 * pixelDrawer.getLfb().getResolutionX() + x1;

        // Determine special cases
        if (dy > 0)
            drawLineMajorAxis(startPixel, 1, pixelDrawer.getLfb().getResolutionX(), dx, dy, color);
        else if (dy < 0)
            drawLineMajorAxis(startPixel, 1, -pixelDrawer.getLfb().getResolutionX(), dx, -dy, color);
        else
            drawLineSingleAxis(startPixel, 1, dx, color);
    }
        // else the Y axis is the major axis
    else {
        // if y2 < y1, flip the points to have fewer special cases
        if (dy < 0) {
            dx *= -1;
            dy *= -1;
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        // Get the address of the pixel at (x1,y1)
        uint32_t startPixel = y1 * pixelDrawer.getLfb().getResolutionX() + x1;

        // determine special cases
        if (dx > 0)
            drawLineMajorAxis(startPixel, pixelDrawer.getLfb().getResolutionX(), 1, dy, dx, color);
        else if (dx < 0)
            drawLineMajorAxis(startPixel, pixelDrawer.getLfb().getResolutionX(), -1, dy, -dx, color);
        else
            drawLineSingleAxis(startPixel, pixelDrawer.getLfb().getResolutionX(), dy, color);
    }
}

void LineDrawer::drawLineMajorAxis(uint32_t pixel, int32_t majorAxisPixelMovement, int32_t minorAxisPixelMovement, int32_t dx, int32_t dy, const Color &color) {
    // Calculate some constants
    const int32_t dx2 = dx * 2;
    const int32_t dy2 = dy * 2;
    const int32_t diffDy2Dx2 = dy2 - dx2;

    // Calculate the starting error value
    auto error = dy2 - dx;

    // Draw the first pixel
    pixelDrawer.drawPixel(pixel, color);

    // lLop across the major axis
    while (dx--) {
        // Move on major axis and minor axis
        if (error > 0) {
            pixel += majorAxisPixelMovement + minorAxisPixelMovement;;
            error += diffDy2Dx2;
        }
            // move on major axis only
        else {
            pixel += majorAxisPixelMovement;
            error += dy2;
        }

        // draw the next pixel
        pixelDrawer.drawPixel(pixel, color);
    }
}

void LineDrawer::drawLineSingleAxis(uint32_t pixel, int32_t majorAxisPixelMovement, int32_t dx, const Color &color) {
    // Draw the first pixel
    pixelDrawer.drawPixel(pixel, color);

    // loop across the major axis and draw the rest of the pixels
    while (dx--) {
        pixel += majorAxisPixelMovement;
        pixelDrawer.drawPixel(pixel, color);
    };

}

void LineDrawer::swap(uint16_t *a, uint16_t *b) {
    uint32_t h = *a;
    *a = *b;
    *b = h;
}

int32_t LineDrawer::abs(int32_t a) {
    if (a < 0) {
        return -a;
    }

    return a;
}

}