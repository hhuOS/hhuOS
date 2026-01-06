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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_FONT_H
#define HHUOS_LIB_UTIL_GRAPHIC_FONT_H

#include <stdint.h>

namespace Util {
namespace Graphic {

/// Represents a monochrome font, consisting of binary data.
/// Each bit represents a pixel. The whole font data is stored in a single array.
/// The starting index of a character sprite can be calculated with
/// ('character width' / 8) * 'character height' * 'character'.
/// Fonts based on this class can be found in the "fonts" directory.
///
/// Based on work by Jakob Falke, oostubs (https://gitlab.cs.fau.de/um15ebek/oostubs)
class Font {

public:
    /// Create a new font instance based on the given binary data.
    /// The instance does not take ownership of the font data, so it must
    /// remain valid for the lifetime of the Font instance.
    /// Usually, font data is stored in a static array.
    Font(const uint8_t charWidth, const uint8_t charHeight, const uint8_t *fontData) :
        charWidth(charWidth), charHeight(charHeight), charMemSize((charWidth + 8 - 1) / 8 * charHeight),
        fontData(fontData) {}

    /// Get a pointer to the binary data of a given character.
    const uint8_t* getChar(uint8_t c) const {
        return &fontData[charMemSize * c];
    }

    /// Get the character width in pixels.
    uint8_t getCharWidth() const {
        return charWidth;
    }

    /// Get the character height in pixels.
    uint8_t getCharHeight() const {
        return charHeight;
    }

private:

    const uint8_t charWidth;
    const uint8_t charHeight;
    const uint8_t charMemSize;
    const uint8_t *fontData;
};

}
}

#endif
