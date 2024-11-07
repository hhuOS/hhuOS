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

#ifndef HHUOS_FONT_H
#define HHUOS_FONT_H

#include <stdint.h>

namespace Util::Graphic {

/**
 * Represents a monochrome font, consisting of binary data. Each bit represents a pixel. The whole font data is stored in a single array.
 * The starting index of a character sprite can be calculated with ('character width' / 8) * 'character height' * 'character'
 *
 * Based on work by Jakob Falke, oostubs (https://gitlab.cs.fau.de/um15ebek/oostubs)
 */
class Font {

public:
    /**
     * Constructor.
     *
     * @param charWidth The width (in pixel) of each character
     * @param charHeight The height (in pixel) of each character
     * @param fontData The binary data
     */
    Font(uint8_t charWidth, uint8_t charHeight, uint8_t *fontData);

    /**
     * Assignment operator.
     */
    Font& operator=(const Font &other) = delete;

    /**
     * Copy Constructor.
     */
    Font(const Font &copy) = delete;

    /**
     * Destructor.
     */
    ~Font() = default;

    /**
     * Get a pointer into the binary, that points at the start of a given character.
     *
     * @param c The character
     * @return The pointer
     */
    [[nodiscard]] uint8_t *getChar(uint8_t c) const;

    /**
     * Get the character width.
     *
     * @return The character width
     */
    [[nodiscard]] uint8_t getCharWidth() const;

    /**
     * Get the character height.
     *
     * @return The character height
     */
    [[nodiscard]] uint8_t getCharHeight() const;

    /**
     * Get a font that is suitable for a given vertical display resolution.
     *
     * @return A reference to the font
     */
    [[nodiscard]] static const Font& getFontForResolution(uint16_t resolutionY);

private:

    const uint8_t charWidth;
    const uint8_t charHeight;
    const uint8_t charMemSize;
    uint8_t *fontData;
};

}

#endif
