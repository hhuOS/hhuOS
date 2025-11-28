/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef __LinearFrameBuffer_include__
#define __LinearFrameBuffer_include__

#include <stdint.h>

#include "Color.h"
#include "lib/util/base/Address.h"

namespace Util {
namespace Graphic {
class Font;
}  // namespace Graphic

namespace Io {
class File;
}  // namespace File
}  // namespace Util

namespace Util::Graphic {

/**
 * Wraps a chunk of memory, that can be used as a linear frame buffer.
 */
class LinearFrameBuffer {

public:

    enum Request {
        SET_RESOLUTION
    };

    /**
     * Constructor.
     */
    LinearFrameBuffer(void *virtualAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch);

    /**
     * Constructor.
     */
    LinearFrameBuffer(uint32_t physicalAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch);

    static LinearFrameBuffer open(Io::File &file);

    /**
     * Assignment operator.
     */
     LinearFrameBuffer& operator=(const LinearFrameBuffer &other) = delete;

    /**
     * Destructor.
     */
    virtual ~LinearFrameBuffer();

    /**
     * Get the horizontal resolution.
     *
     * @return The horizontal resolution
     */
    [[nodiscard]] uint16_t getResolutionX() const;

    /**
     * Get the vertical resolution.
     *
     * @return The vertical resolution
     */
    [[nodiscard]] uint16_t getResolutionY() const;

    /**
     * Get the color colorDepth.
     *
     * @return The color colorDepth
     */
    [[nodiscard]] uint8_t getColorDepth() const;

    /**
     * Get the buffer's pitch.
     *
     * @return The pitch
     */
    [[nodiscard]] uint16_t getPitch() const;

    /**
     * Get the amount of bytes per pixel.
     *
     * @return The amount of bytes per pixel
     */
    [[nodiscard]] uint8_t getBytesPerPixel() const {
        return (colorDepth + 7) / 8;
    }

    /**
     * Get the buffer address.
     *
     * @return The buffer address
     */
    [[nodiscard]] const Address& getBuffer() const;

    /**
     * Read the color of a pixel at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param color A reference to the variable, that the pixel's color will be written to
     */
    [[nodiscard]] Color readPixel(uint16_t x, uint16_t y) const;

    [[nodiscard]] bool isCompatibleWith(const LinearFrameBuffer &other) const;

    void clear() const;

    void drawPixel(uint16_t x, uint16_t y, const Color &color) const;

    void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color &color) const;

    void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) const;

    void drawSquare(uint16_t x, uint16_t y, uint16_t size, const Color &color) const;

    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) const;

    void fillSquare(uint16_t x, uint16_t y, uint16_t size, const Color &color) const;

    void drawCircle(uint16_t x, uint16_t y, uint16_t radius, const Color& color) const;

    void fillCircle(uint16_t x, uint16_t y, uint16_t radius, const Color& color) const;

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
     * @param string The string
     * @param fgColor The foreground color
     * @param bgColor The background color
     */
    void drawString(const Font &font, uint16_t x, uint16_t y, const char *string, const Color &fgColor, const Color &bgColor) const;

    /**
     * Scroll the buffer upwards by a given amount of pixel lines.
     *
     * @param lineCount The amount of pixel lines to scroll up
     */
    void scrollUp(uint16_t lineCount, bool clearBelow) const;

private:

    /**
     * Copy Constructor.
     */
    LinearFrameBuffer(const LinearFrameBuffer &copy) = default;

    static void* mapBuffer(void *physicalAddress, uint16_t resolutionY, uint16_t pitch);

    void drawLineMajorAxis(int32_t x, int32_t y, int8_t xMovement, int8_t yMovement, int32_t dx, int32_t dy, bool majorAxisX, const Color &color) const;

    void drawLineSingleAxis(int32_t x, int32_t y, int8_t movement, int32_t dx, bool majorAxisX, const Color &color) const;

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
    void drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &fgColor, const Color &bgColor, const uint8_t *bitmap) const;

    Address buffer;

    uint16_t resolutionX = 0;
    uint16_t resolutionY = 0;
    uint8_t colorDepth = 0;
    uint16_t pitch = 0;
};

}

#endif
