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

#include "lib/util/base/Exception.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/interface.h"
#include "LinearFrameBuffer.h"

#include <lib/util/math/Math.h>

#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/base/String.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Constants.h"
#include "lib/util/collection/Array.h"

void swap(int32_t *a, int32_t *b) {
    auto h = *a;
    *a = *b;
    *b = h;
}

namespace Util::Graphic {

LinearFrameBuffer::LinearFrameBuffer(void *virtualAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        buffer(virtualAddress), resolutionX(resolutionX), resolutionY(resolutionY), colorDepth(colorDepth), pitch(pitch) {
    switch (colorDepth) {
        case 15:
            drawFunction = &drawPixel15Bit;
        break;
        case 16:
            drawFunction = &drawPixel16Bit;
        break;
        case 24:
            drawFunction = &drawPixel24Bit;
        break;
        case 32:
            drawFunction = &drawPixel32Bit;
        break;
        default:
            Exception::throwException(Exception::INVALID_ARGUMENT, "PixelDrawer: Illegal color depth!");
    }
}

LinearFrameBuffer::LinearFrameBuffer(uint32_t physicalAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        LinearFrameBuffer(mapBuffer(reinterpret_cast<void*>(physicalAddress), resolutionY, pitch), resolutionX, resolutionY, colorDepth, pitch) {}

LinearFrameBuffer LinearFrameBuffer::open(Io::File &file) {
    if (!file.exists()) {
        Exception::throwException(Exception::INVALID_ARGUMENT, "LinearFrameBuffer: File does not exist!");
    }

    auto stream = Io::FileInputStream(file);
    bool endOfFile;

    const auto addressString = stream.readLine(endOfFile);
    const auto resolutionString = stream.readLine(endOfFile);
    const auto pitchString = stream.readLine(endOfFile);

    const auto colorDepthSplit = resolutionString.split("@");
    const auto resolutionSplit = colorDepthSplit[0].split("x");

    const auto address = Util::String::parseInt(static_cast<const char*>(addressString));
    const auto resolutionX = Util::String::parseInt(static_cast<const char*>(resolutionSplit[0]));
    const auto resolutionY = Util::String::parseInt(static_cast<const char*>(resolutionSplit[1]));
    const auto colorDepth = Util::String::parseInt(static_cast<const char*>(colorDepthSplit[1]));
    const auto pitch = Util::String::parseInt(static_cast<const char*>(pitchString));
    auto *buffer = mapBuffer(reinterpret_cast<void*>(address), resolutionY, pitch);

    return LinearFrameBuffer(buffer, resolutionX, resolutionY, colorDepth, pitch);
}

LinearFrameBuffer::~LinearFrameBuffer() {
    delete reinterpret_cast<uint8_t*>(buffer.get());
}

uint16_t LinearFrameBuffer::getResolutionX() const {
    return resolutionX;
}

uint16_t LinearFrameBuffer::getResolutionY() const {
    return resolutionY;
}

uint8_t LinearFrameBuffer::getColorDepth() const {
    return colorDepth;
}

uint16_t LinearFrameBuffer::getPitch() const {
    return pitch;
}

const Address<uint32_t> &LinearFrameBuffer::getBuffer() const {
    return buffer;
}

Color LinearFrameBuffer::readPixel(uint16_t x, uint16_t y) const {
    if (x > resolutionX - 1 || y > resolutionY - 1) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "LinearFrameBuffer: Trying to read a pixel out of bounds!");
    }

    auto bpp = static_cast<uint8_t>(colorDepth == 15 ? 16 : colorDepth);
    auto address = buffer.add(x * (bpp / 8) + y * pitch);

    return Color::fromRGB(*reinterpret_cast<uint32_t*>(address.get()), colorDepth);
}

void LinearFrameBuffer::clear() const {
    buffer.setRange(0, getPitch() * getResolutionY());
}

void LinearFrameBuffer::drawPixel(uint16_t x, uint16_t y, const Color &color) const {
    // Pixels outside the visible area won't be drawn
    if (x >= resolutionX || y >= resolutionY) {
        return;
    }

    // Invisible pixels won't be drawn
    if (color.getAlpha() == 0) {
        return;
    }

    // Blend if necessary and draw pixel
    if (color.getAlpha() < 255) {
        drawFunction(reinterpret_cast<uint8_t*>(buffer.get()), pitch, x, y, readPixel(x, y).blend(color));
    } else {
        drawFunction(reinterpret_cast<uint8_t*>(buffer.get()), pitch, x, y, color);
    }
}

void LinearFrameBuffer::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color &color) const {
    // Calculate our deltas
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;

    // If the x-axis is the major axis
    if (Math::absolute(dx) >= Math::absolute(dy)) {
        // If x2 < x1, flip the points to have fewer special cases
        if (dx < 0) {
            dx *= -1;
            dy *= -1;
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        // Determine special cases
        if (dy > 0) {
            drawLineMajorAxis(x1, y1, 1, 1, dx, dy, true, color);
        } else if (dy < 0) {
            drawLineMajorAxis(x1, y1, 1, -1, dx, -dy, true, color);
        } else {
            drawLineSingleAxis(x1, y1, 1, dx, true, color);
        }
    }
        // else the y-axis is the major axis
    else {
        // if y2 < y1, flip the points to have fewer special cases
        if (dy < 0) {
            dx *= -1;
            dy *= -1;
            swap(&x1, &x2);
            swap(&y1, &y2);
        }

        // determine special cases
        if (dx > 0) {
            drawLineMajorAxis(x1, y1, 1, 1, dy, dx, false, color);
        } else if (dx < 0) {
            drawLineMajorAxis(x1, y1, -1, 1, dy, -dx, false, color);
        } else {
            drawLineSingleAxis(x1, y1, 1, dy, false, color);
        }
    }
}

void LinearFrameBuffer::drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) const {
    drawLine(x, y, x + width, y, color);
    drawLine(x, y + height, x + width, y + height, color);
    drawLine(x, y, x, y + height, color);
    drawLine(x + width, y, x + width, y + height, color);
}

void LinearFrameBuffer::drawSquare(uint16_t x, uint16_t y, uint16_t size, const Color &color) const {
    drawRectangle(x, y, size, size, color);
}

void LinearFrameBuffer::fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &color) const {
    const uint16_t endX = x + width;
    const uint16_t endY = y + height;

    for (uint16_t i = y; i < endY; i++) {
        drawLine(x, i, endX, i, color);
    }
}

void LinearFrameBuffer::fillSquare(uint16_t x, uint16_t y, uint16_t size, const Color &color) const {
    fillRectangle(x, y, size, size, color);
}

void LinearFrameBuffer::drawChar(const Font &font, uint16_t x, uint16_t y, char c, const Color &fgColor, const Color &bgColor) const {
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(c));
}

void LinearFrameBuffer::drawString(const Font &font, uint16_t x, uint16_t y, const char *string, const Color &fgColor, const Color &bgColor) const {
    for (uint32_t i = 0; string[i] >= 0x20 && string[i] <= 0x7e; ++i) {
        drawChar(font, x, y, string[i], fgColor, bgColor);
        x += font.getCharWidth();
    }
}

void LinearFrameBuffer::scrollUp(uint16_t lineCount, bool clearBelow) const {
    // Move screen buffer upwards by the given amount of lines
    auto source = buffer.add(pitch * lineCount);
    buffer.copyRange(source, pitch * (resolutionY - lineCount));

    // Clear lower part of the screen
    if (clearBelow) {
        auto clear = buffer.add(pitch * (resolutionY - lineCount));
        clear.setRange(0, pitch * lineCount);
    }
}

void LinearFrameBuffer::drawLineMajorAxis(int32_t x, int32_t y, int8_t xMovement, int8_t yMovement, int32_t dx, int32_t dy, bool majorAxisX, const Color &color) const {
    // Calculate some constants
    const int32_t dx2 = dx * 2;
    const int32_t dy2 = dy * 2;
    const int32_t diffDy2Dx2 = dy2 - dx2;

    // Calculate the starting error value
    auto error = dy2 - dx;

    // Draw the first pixel
    drawPixel(x, y, color);

    // lLop across the major axis
    while (dx-- > 0) {
        if (error > 0) {
            // Move on major axis and minor axis
            x += xMovement;
            y += yMovement;
            error += diffDy2Dx2;
        } else {
            // Move on major axis only
            if (majorAxisX) {
                x += xMovement;
            } else {
                y += yMovement;
            }

            error += dy2;
        }

        // draw the next pixel
        drawPixel(x, y, color);
    }
}

void LinearFrameBuffer::drawLineSingleAxis(int32_t x, int32_t y, int8_t movement, int32_t dx, bool majorAxisX, const Color &color) const {
    // Draw the first pixel
    drawPixel(x, y, color);

    // loop across the major axis and draw the rest of the pixels
    while (dx-- > 0) {
        if (majorAxisX) {
            x += movement;
        } else {
            y += movement;
        }

        drawPixel(x, y, color);
    }
}

void LinearFrameBuffer::drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const Color &fgColor, const Color &bgColor, uint8_t *bitmap) const {
    const uint32_t widthInBytes = width / 8 + ((width % 8 != 0) ? 1 : 0);

    for (uint32_t offsetY = 0; offsetY < height; ++offsetY) {
        uint32_t posX = x;
        const uint32_t posY = y + offsetY;

        for (uint32_t xByte = 0; xByte < widthInBytes; ++xByte) {
            const uint8_t bitLimit = (xByte == widthInBytes - 1) ? width % 8 : 0;
            for (int8_t bit = 7; bit >= bitLimit; --bit) {
                if ((1 << bit) & *bitmap) {
                    drawPixel(posX, posY, fgColor);
                } else {
                    drawPixel(posX, posY, bgColor);
                }

                posX++;
            }

            bitmap++;
        }
    }
}

void LinearFrameBuffer::drawPixel15Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    const auto offset = x + y * (pitch / 2);
    reinterpret_cast<uint16_t*>(buffer)[offset] = color.getRGB15();
}

void LinearFrameBuffer::drawPixel16Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    const auto offset = x + y * (pitch / 2);
    reinterpret_cast<uint16_t*>(buffer)[offset] = color.getRGB16();
}

void LinearFrameBuffer::drawPixel24Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    uint32_t rgbColor = color.getRGB24();
    const auto offset = x * 3 + y * pitch;

    buffer[offset] = rgbColor & 0xff;
    buffer[offset + 1] = (rgbColor >> 8) & 0xff;
    buffer[offset + 2] = (rgbColor >> 16) & 0xff;
}

void LinearFrameBuffer::drawPixel32Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    const auto offset = x + y * (pitch / 4);
    reinterpret_cast<uint32_t*>(buffer)[offset] = color.getRGB32();
}

void* LinearFrameBuffer::mapBuffer(void *physicalAddress, uint16_t resolutionY, uint16_t pitch) {
    if (reinterpret_cast<uint32_t>(physicalAddress) % Util::PAGESIZE != 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "LinearFrameBuffer: Physical address is not page aligned!");
    }

    const auto size = resolutionY * pitch;
    const auto pageCount = size % Util::PAGESIZE == 0 ? (size / Util::PAGESIZE) : (size / Util::PAGESIZE) + 1;
    auto *virtualAddress = mapIO(physicalAddress, pageCount);

    return virtualAddress;
}

uint8_t LinearFrameBuffer::getBytesPerPixel() const {
    return (colorDepth == 15 ? 16 : colorDepth) / 8;
}

bool LinearFrameBuffer::isCompatibleWith(const LinearFrameBuffer &other) const {
    return resolutionX == other.resolutionX && resolutionY == other.resolutionY && colorDepth == other.colorDepth && pitch == other.pitch;
}

}