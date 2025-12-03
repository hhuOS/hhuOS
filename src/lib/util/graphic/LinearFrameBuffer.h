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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_LINEARFRAMEBUFFER_H
#define HHUOS_LIB_UTIL_GRAPHIC_LINEARFRAMEBUFFER_H

#include <stdint.h>

#include "util/base/Address.h"
#include "util/io/file/File.h"
#include "util/graphic/Color.h"
#include "util/graphic/Font.h"

namespace Util {
namespace Graphic {

/// Represents a linear frame buffer that can be used to draw graphics on the screen.
class LinearFrameBuffer {

public:
    /// Requests that can be issued to the linear frame buffer via `Util::Io::File::controlFile()` calls.
    /// This can be used to manipulate the hardware frame buffer device from user space,
    /// via the "/device/lfb" file.
    enum Request {
        /// Set the resolution of the hardware frame buffer.
        SET_RESOLUTION
    };

    /// Create a new linear frame buffer instance from a virtual address.
    /// This constructor should be used if the frame buffer is already mapped into the virtual address space,
    /// or if the frame buffer is not mapped to a hardware device (e.g. software frame buffer for double buffering).
    LinearFrameBuffer(void *virtualAddress, const uint16_t resolutionX, const uint16_t resolutionY,
        const uint8_t colorDepth, const uint16_t pitch) : buffer(virtualAddress), resolutionX(resolutionX),
        resolutionY(resolutionY), colorDepth(colorDepth), pitch(pitch) {}

    /// Create a new linear frame buffer instance from a physical address.
    /// This constructor maps the frame buffer from the given physical address into the virtual address space.
    /// This constructor should be used for hardware frame buffers. In user space, the physical address is typically
    /// obtained from the frame buffer device file (e.g. "/device/lfb"). However, it is more convenient to use the
    /// constructor that takes a file as argument instead.
    LinearFrameBuffer(const size_t physicalAddress, const uint16_t resolutionX, const uint16_t resolutionY,
        const uint8_t colorDepth, const uint16_t pitch) : LinearFrameBuffer(
        mapBuffer(physicalAddress, resolutionY, pitch), resolutionX, resolutionY, colorDepth, pitch) {}

    /// Create a new linear frame buffer instance from a device file (typically "/device/lfb").
    /// This constructor reads the frame buffer parameters from the file and maps the frame buffer
    /// into the virtual address space.
    explicit LinearFrameBuffer(const Io::File &file);

    /// LinearFrameBuffer is not copyable, since it manages a buffer on the heap, so the copy constructor is deleted.
    LinearFrameBuffer(const LinearFrameBuffer &copy) = delete;

    /// LinearFrameBuffer is not copyable, since it manages a buffer on the heap, so the assignment operator is deleted.
    LinearFrameBuffer& operator=(const LinearFrameBuffer &other) = delete;

    /// Destroy the linear frame buffer instance and delete the mapped buffer.
    virtual ~LinearFrameBuffer() {
        delete reinterpret_cast<uint8_t*>(buffer.get());
    }

    /// Get the horizontal resolution of the frame buffer in pixels.
    uint16_t getResolutionX() const {
        return resolutionX;
    }

    /// Get the vertical resolution of the frame buffer in pixels.
    uint16_t getResolutionY() const {
        return resolutionY;
    }

    /// Get the color depth of the frame buffer in bits per pixel.
    uint8_t getColorDepth() const {
        return colorDepth;
    }

    /// Get the pitch of the frame buffer (i.e. the number of bytes per row).
    /// This must not necessarily be equal to (resolutionX * colorDepth / 8),
    /// since there may be padding bytes at the end of each row.
    uint16_t getPitch() const {
        return pitch;
    }

    /// Get the virtual address of the frame buffer.
    /// This can be used to directly manipulate the frame buffer memory.
    const Address& getBuffer() const {
        return buffer;
    }

    /// Check whether this frame buffer is compatible with another frame buffer.
    /// This is mainly useful for double buffering, to check whether the back buffer
    /// can simply replace the front buffer.
    bool isCompatibleWith(const LinearFrameBuffer &other) const {
        return resolutionX == other.resolutionX &&
            resolutionY == other.resolutionY &&
            colorDepth == other.colorDepth &&
            pitch == other.pitch;
    }

    /// Clear the frame buffer by setting all pixels to black.
    void clear() const;

    /// Read the color of a pixel at the given coordinates.
    /// CAUTION: This operation is slow on hardware frame buffers, since it requires reading from the video memory.
    /// It is recommended to use double buffering if pixel reads are required.
    Color readPixel(uint16_t x, uint16_t y) const;

    /// Set the pixel at the given coordinates to the specified color.
    void drawPixel(uint16_t x, uint16_t y, const Color &color) const;

    /// Draw a line from (x1, y1) to (x2, y2) with the specified color using Bresenham's line algorithm.
    void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const Color &color) const;

    /// Draw the outline of a rectangle with the upper-left corner at (x, y),
    /// the specified width and height, and the given color.
    void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) const;

    /// Draw the outline of a square with the upper-left corner at (x, y),
    /// the specified size, and the given color.
    void drawSquare(uint16_t x, uint16_t y, uint16_t size, const Color &color) const;

    /// Draw a filled rectangle with the upper-left corner at (x, y),
    /// the specified width and height, and the given color.
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) const;

    /// Draw a filled square with the upper-left corner at (x, y),
    /// the specified size, and the given color.
    void fillSquare(uint16_t x, uint16_t y, uint16_t size, const Color &color) const;

    /// Draw the outline of a circle centered at (x, y) with the specified radius and color.
    void drawCircle(uint16_t x, uint16_t y, uint16_t radius, const Color& color) const;

    /// Draw a filled circle centered at (x, y) with the specified radius and color.
    void fillCircle(uint16_t x, uint16_t y, uint16_t radius, const Color& color) const;

    /// Draw a character of the given bitmap font with the upper-left corner at (x, y),
    /// using the specified foreground and background colors.
    void drawChar(const Font &font, uint16_t x, uint16_t y, char c, const Color &fgColor, const Color &bgColor) const;

    /// Draw a string of characters of the given bitmap font with the upper-left corner at (x, y),
    /// using the specified foreground and background colors.
    /// This method does not handle line wrapping and simply draws the characters in a single line.
    void drawString(const Font &font, uint16_t x, uint16_t y, const char *string,
        const Color &fgColor,const Color &bgColor) const;

    /// Scroll the frame buffer content up by the specified number of lines.
    /// If `clearBelow` is true, the area below the scrolled content is cleared to black.
    void scrollUp(uint16_t lineCount, bool clearBelow) const;

private:

    static uint8_t* mapBuffer(uint32_t physicalAddress, uint16_t resolutionY, uint16_t pitch);

    void drawLineMajorAxis(uint16_t x, uint16_t y, int8_t xMovement, int8_t yMovement, int32_t dx, int32_t dy,
        bool majorAxisX, const Color &color) const;

    void drawLineSingleAxis(uint16_t x, uint16_t y, int8_t movement, int32_t dx, bool majorAxisX,
        const Color &color) const;

    void drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &fgColor,
        const Color &bgColor, const uint8_t *bitmap) const;

    Address buffer;

    uint16_t resolutionX = 0;
    uint16_t resolutionY = 0;
    uint8_t colorDepth = 0;
    uint16_t pitch = 0;
};

}
}

#endif
