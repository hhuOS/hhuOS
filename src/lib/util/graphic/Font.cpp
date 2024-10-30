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

#include "Font.h"
#include "lib/util/graphic/font/Mini4x6.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/font/Terminal8x16.h"

namespace Util::Graphic {

Font::Font(uint8_t charWidth, uint8_t charHeight, uint8_t *fontData) : charWidth(charWidth), charHeight(charHeight), charMemSize((charWidth / 8 + (charWidth % 8 == 0 ? 0 : 1)) * charHeight), fontData(fontData) {}

uint8_t Font::getCharWidth() const {
    return charWidth;
}

uint8_t Font::getCharHeight() const {
    return charHeight;
}

uint8_t* Font::getChar(uint8_t c) const {
    return &fontData[charMemSize * c];
}

const Font& Font::getFontForResolution(uint16_t resolutionY) {
    if (resolutionY < 350) {
        return Fonts::MINI_4x6;
    } else if (resolutionY < 500) {
        return Fonts::TERMINAL_8x8;
    } else {
        return Fonts::TERMINAL_8x16;
    }
}

}