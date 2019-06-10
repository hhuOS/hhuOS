/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "lib/math/Complex.h"
#include "Mandelbrot.h"

void Mandelbrot::drawMandelbrotNoSSE(float offsetX, float offsetY, float zoom) {
    bool notInSet = false;
    uint32_t i;

    for (uint16_t y = 0; y < Mandelbrot::properties.height; y++) {
        for (uint16_t x = 0; x < Mandelbrot::properties.width; x++) {

            Complex c((-2.0F + realBase * x) / zoom + offsetX,
                      (-2.0F + imaginaryBase * y) / zoom + offsetY);

            Complex z(0.0, 0.0);

            for (i = 0; i < MAX_VALUE; i++) {

                z = z * z + c;

                if (z.squaredAbsolute() >= 4.0) {

                    notInSet = true;

                    break;
                }
            }

            if(notInSet) {
                lfb->drawPixel(x, y, gradient.getColor(i % MAX_COLORS));
            } else {
                lfb->drawPixel(x, y, Colors::BLACK);
            }

            notInSet = false;
        }
    }
}