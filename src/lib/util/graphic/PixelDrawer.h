/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_PIXELDRAWER_H
#define HHUOS_PIXELDRAWER_H

#include <cstdint>

namespace Util {
namespace Graphic {
class Color;
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

/**
 * Draws pixels on a LinearFrameBuffer.
 */
class PixelDrawer {

public:
    /**
     * Constructor.
     *
     * @param lfb The linear frame buffer on which to draw pixels.
     */
    explicit PixelDrawer(const LinearFrameBuffer &lfb);

    /**
     * Copy Constructor.
     */
    PixelDrawer(const PixelDrawer &copy) = delete;

    /**
     * Assignment operator.
     */
    PixelDrawer& operator=(const PixelDrawer & other) = delete;

    /**
     * Destructor.
     */
    ~PixelDrawer() = default;

    /**
     * Draw a pixel at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param color The color
     */
    void drawPixel(uint16_t x, uint16_t y, const Color &color) const;

private:

    static void drawPixel15Bit(uint8_t *buffer, uint16_t width, uint16_t x, uint16_t y, const Color &color);

    static void drawPixel16Bit(uint8_t *buffer, uint16_t width, uint16_t x, uint16_t y, const Color &color);

    static void drawPixel24Bit(uint8_t *buffer, uint16_t width, uint16_t x, uint16_t y, const Color &color);

    static void drawPixel32Bit(uint8_t *buffer, uint16_t width, uint16_t x, uint16_t y, const Color &color);

    const LinearFrameBuffer &lfb;
    uint8_t *const lfbBuffer;
    const uint16_t lfbWidth;

    void (*drawFunction)(uint8_t *buffer, uint16_t width, uint16_t x, uint16_t y, const Color &color);
};

}

#endif
