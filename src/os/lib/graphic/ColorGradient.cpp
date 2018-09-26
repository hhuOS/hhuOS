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

#include "ColorGradient.h"
#include "Colors.h"

ColorGradient::ColorGradient(uint32_t numColors) : numColors(numColors) {
    colors = new Color[numColors];

    for(uint32_t i = 0; i < numColors; i++) {
        colors[i] = calculateColor(static_cast<float>(i) / static_cast<float>(numColors));
    }
}

ColorGradient::~ColorGradient() {
    delete[] colors;
}

Color ColorGradient::calculateColor(double value) {
    auto h = static_cast<uint32_t>(value * 256 * 6);
    auto x = static_cast<uint8_t>(h % 256);

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    switch(h / 256) {
        case 0:
            r = 255;
            g = x;
            break;
        case 1:
            g = 255;
            r = static_cast<uint8_t>(255 - x);
            break;
        case 2:
            g = 255;
            b = x;
            break;
        case 3:
            b = 255;
            g = static_cast<uint8_t>(255 - x);
            break;
        case 4:
            b = 255;
            r = x;
            break;
        case 5:
            r = 255;
            g = static_cast<uint8_t>(255 - x);
            break;
        default:
            break;
    }

    return Color(r, g, b);
}

Color &ColorGradient::getColor(uint32_t num) {
    if(num >= numColors) {
        return Colors::BLACK;
    }

    return colors[num];
}
