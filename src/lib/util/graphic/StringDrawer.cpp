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

#include "StringDrawer.h"

#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/PixelDrawer.h"

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

StringDrawer::StringDrawer(const PixelDrawer &pixelDrawer) : pixelDrawer(pixelDrawer) {}

void StringDrawer::drawChar(const Font &font, uint16_t x, uint16_t y, char c, const Color &fgColor, const Color &bgColor) const {
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(c));
}

void StringDrawer::drawString(const Font &font, uint16_t x, uint16_t y, const char *string, const Color &fgColor, const Color &bgColor) const {
    for (uint32_t i = 0; string[i] >= 0x20 && string[i] <= 0x7e; ++i) {
        drawChar(font, x, y, string[i], fgColor, bgColor);
        x += font.getCharWidth();
    }
}

void StringDrawer::drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &fgColor, const Color &bgColor, uint8_t *bitmap) const {
    auto widthInBytes = width / 8 + ((width % 8 != 0) ? 1 : 0);

    for (uint32_t offsetY = 0; offsetY < height; ++offsetY) {
        uint32_t posX = x;
        uint32_t posY = y + offsetY;

        for (int32_t xb = 0; xb < widthInBytes; ++xb) {
            for (int8_t src = 7; src >= 0; --src) {
                if ((1 << src) & *bitmap)
                    pixelDrawer.drawPixel(posX, posY, fgColor);
                else
                    pixelDrawer.drawPixel(posX, posY, bgColor);
                posX++;
            }
            bitmap++;
        }
    }
}

}