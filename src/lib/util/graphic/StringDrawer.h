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

#ifndef HHUOS_STRINGDRAWER_H
#define HHUOS_STRINGDRAWER_H

#include <cstdint>

namespace Util {
namespace Graphic {
class Color;
class Font;
class PixelDrawer;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

/**
 * Uses a PixelDrawer to draw characters and strings.
 */
class StringDrawer {

public:

    /**
     * Constructor.
     *
     * @param pixelDrawer The PixelDrawer to use for drawing characters
     */
    explicit StringDrawer(const PixelDrawer &pixelDrawer);

    /**
     * Copy Constructor.
     */
    StringDrawer(const StringDrawer &copy) = delete;

    /**
     * Assignment operator.
     */
    StringDrawer &operator=(const StringDrawer &other) = delete;

    /**
     * Destructor.
     */
    ~StringDrawer() = default;

    /**
     * Draw a character at a given position.
     *
     * @param font The font
     * @param x The x coordinate of upper left corner
     * @param y The y coordinate of upper left corner
     * @param c The character
     * @param fgColor The foreground color
     * @param bgColor The background color
     */
    void drawChar(const Font &font, uint16_t x, uint16_t y, char c, const Color &fgColor, const Color &bgColor) const;

    /**
     * Draw a null-terminated string at a given position.
     *
     * @param font The font
     * @param x The x coordinate of upper left corner
     * @param y The y coordinate of upper left corner
     * @param s The string
     * @param fgColor The foreground color
     * @param bgColor The background color
     */
    void drawString(const Font &font, uint16_t x, uint16_t y, const char *string, const Color &fgColor, const Color &bgColor) const;

private:

    /**
     * Draw a monochrome bitmap at a given position. Each pixel to be drawn is represented by a single bit in the bitmap.
     * If the bit is set to 1, the foreground color is used to draw the pixel, else the background color is used.
     *
     * @param x The x coordinate of upper left corner
     * @param y The y coordinate of upper left corner
     * @param width The bitmap's width
     * @param height The bitmap's height
     * @param fgColor The foreground color
     * @param bgColor The background color
     * @param bitmap The bitmap's data.
     */
    void drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &fgColor, const Color &bgColor, uint8_t *bitmap) const;

    const PixelDrawer &pixelDrawer;
};

}

#endif
