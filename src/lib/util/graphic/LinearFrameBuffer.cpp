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

#include "LinearFrameBuffer.h"

#include "util/base/Address.h"
#include "util/base/Constants.h"
#include "util/base/Panic.h"
#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/graphic/Color.h"
#include "util/graphic/Font.h"
#include "util/io/stream/FileInputStream.h"
#include "util/io/file/File.h"
#include "util/math/Math.h"
#include "lib/interface.h"

namespace Util {
namespace Graphic {

void swap(uint16_t *a, uint16_t *b) {
    const auto h = *a;
    *a = *b;
    *b = h;
}

LinearFrameBuffer::LinearFrameBuffer(const Io::File &file) {
    if (!file.exists()) {
        Panic::fire(Panic::INVALID_ARGUMENT, "LinearFrameBuffer: File does not exist!");
    }

    Io::FileInputStream stream(file);

    const auto addressString = stream.readLine().content;
    const auto resolutionString = stream.readLine().content;
    const auto pitchString = stream.readLine().content;

    const auto colorDepthSplit = resolutionString.split("@");
    const auto resolutionSplit = colorDepthSplit[0].split("x");

    const auto address = String::parseNumber<size_t>(addressString);

    resolutionX = String::parseNumber<uint16_t>(resolutionSplit[0]);
    resolutionY = String::parseNumber<uint16_t>(resolutionSplit[1]);
    colorDepth = String::parseNumber<uint8_t>(colorDepthSplit[1]);
    pitch = String::parseNumber<uint16_t>(pitchString);
    buffer = Address(mapBuffer(address, resolutionY, pitch));
}

void LinearFrameBuffer::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
    const Color &color) const
{
    // Calculate our deltas
    auto dx = x2 - x1;
    auto dy = y2 - y1;

    if (Math::absolute(dx) >= Math::absolute(dy)) { // If the x-axis is the major axis
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
    } else { // Else the y-axis is the major axis
        // If y2 < y1, flip the points to have fewer special cases
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

void LinearFrameBuffer::drawRectangle(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height,
    const Color &color) const
{
    drawLine(x, y, x + width - 1, y, color);
    drawLine(x, y + height - 1, x + width - 1, y + height - 1, color);
    drawLine(x, y, x, y + height - 1, color);
    drawLine(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void LinearFrameBuffer::drawSquare(const uint16_t x, const uint16_t y, const uint16_t size, const Color &color) const {
    drawRectangle(x, y, size, size, color);
}

void LinearFrameBuffer::fillRectangle(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const Color &color) const {
    const uint16_t endX = x + width - 1;
    const uint16_t endY = y + height -1;

    for (uint16_t i = y; i <= endY; i++) {
        drawLine(x, i, endX, i, color);
    }
}

void LinearFrameBuffer::fillSquare(const uint16_t x, const uint16_t y, const uint16_t size, const Color &color) const {
    fillRectangle(x, y, size, size, color);
}

void LinearFrameBuffer::drawCircle(const uint16_t x, const uint16_t y, const uint16_t radius,
    const Color &color) const
{
    for (int32_t dy = -radius; dy <= radius; dy++) {
        const auto dx = static_cast<int32_t>(Math::sqrt(static_cast<float>(radius * radius - dy * dy)));

        drawPixel(x + dx, y + dy, color);
        drawPixel(x - dx, y + dy, color);
    }
}

void LinearFrameBuffer::fillCircle(const uint16_t x, const uint16_t y, const uint16_t radius,
    const Color &color) const
{
    for (int32_t dy = -radius; dy <= radius; dy++) {
        const auto dxLimit = static_cast<int32_t>(Math::sqrt(static_cast<float>(radius * radius - dy * dy)));

        for (int32_t dx = -dxLimit; dx <= dxLimit; dx++) {
            drawPixel(x + dx, y + dy, color);
        }
    }
}

void LinearFrameBuffer::drawChar(const Font &font, const uint16_t x, const uint16_t y, const char c,
    const Color &fgColor, const Color &bgColor) const
{
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(c));
}

void LinearFrameBuffer::drawString(const Font &font, uint16_t x, const uint16_t y, const char *string,
    const Color &fgColor, const Color &bgColor) const
{
    for (uint32_t i = 0; string[i] >= 0x20 && string[i] <= 0x7e; ++i) {
        drawChar(font, x, y, string[i], fgColor, bgColor);
        x += font.getCharWidth();
    }
}

void LinearFrameBuffer::scrollUp(const uint16_t lineCount, const bool clearBelow) const {
    // Move screen buffer upwards by the given amount of lines
    const auto source = buffer.add(pitch * lineCount);
    buffer.copyRange(source, pitch * (resolutionY - lineCount));

    // Clear lower part of the screen
    if (clearBelow) {
        const auto clear = buffer.add(pitch * (resolutionY - lineCount));
        clear.setRange(0, pitch * lineCount);
    }
}

void LinearFrameBuffer::drawLineMajorAxis(uint16_t x, uint16_t y, const int8_t xMovement, const int8_t yMovement,
    int32_t dx, const int32_t dy, const bool majorAxisX, const Color &color) const
{
    // Calculate some constants
    const auto dx2 = dx * 2;
    const auto dy2 = dy * 2;
    const auto diffDy2Dx2 = dy2 - dx2;

    // Calculate the starting error value
    auto error = dy2 - dx;

    // Draw the first pixel
    drawPixel(x, y, color);

    // Loop across the major axis
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

        // Draw the next pixel
        drawPixel(x, y, color);
    }
}

void LinearFrameBuffer::drawLineSingleAxis(uint16_t x, uint16_t y, const int8_t movement, int32_t dx,
    const bool majorAxisX, const Color &color) const
{
    // Draw the first pixel
    drawPixel(x, y, color);

    // Loop across the major axis and draw the rest of the pixels
    while (dx-- > 0) {
        if (majorAxisX) {
            x += movement;
        } else {
            y += movement;
        }

        drawPixel(x, y, color);
    }
}

void LinearFrameBuffer::drawMonoBitmap(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height,
    const Color &fgColor, const Color &bgColor, const uint8_t *bitmap) const
{
    // Each bit in the bitmap represents a pixel (1 = fgColor, 0 = bgColor)
    // and is used as index into this array for drawing (avoids branching)
    const Color *colors[] = { &bgColor, &fgColor };
    const uint16_t widthInBytes = (width + 7) / 8;

    for (uint16_t offsetY = 0; offsetY < height; ++offsetY) {
        uint16_t posX = x;
        const uint16_t posY = y + offsetY;

        // Iterate over bytes in a row
        for (uint16_t xByte = 0; xByte < widthInBytes; ++xByte) {
            // Handle last byte special (might be not fully used)
            // Multiply with (xByte == widthInBytes - 1) to only apply on last byte (avoids branching)
            const uint8_t bitLimit = (width % 8) * (xByte == widthInBytes - 1);

            // Iterate over bits in a byte
            for (int8_t bit = 7; bit >= bitLimit; --bit) {
                const auto pixel = (*bitmap >> bit) & 0x01;
                drawPixel(posX, posY, *colors[pixel]);

                posX++;
            }

            bitmap++;
        }
    }
}

uint8_t* LinearFrameBuffer::mapBuffer(const uint32_t physicalAddress, const uint16_t resolutionY,
    const uint16_t pitch)
{
    const auto pageOffset = physicalAddress % PAGESIZE;
    const auto sizeWithOffset = pageOffset + pitch * resolutionY;
    const auto pageCount = (sizeWithOffset + PAGESIZE - 1) / PAGESIZE;

    auto *virtualAddress = mapIO(physicalAddress, pageCount);
    return static_cast<uint8_t*>(virtualAddress) + pageOffset;
}

}
}