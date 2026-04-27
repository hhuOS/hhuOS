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

#include "LinearFrameBuffer.h"

#include "Color.h"
#include "Font.h"

#include <interface.h>
#include <util/base/Address.h>
#include <util/base/Constants.h>
#include <util/base/Panic.h>
#include <util/base/String.h>
#include <util/collection/Array.h>
#include <util/io/stream/FileInputStream.h>
#include <util/io/file/File.h>
#include <util/math/Math.h>

namespace Util {
namespace Graphic {

void swap(int32_t *a, int32_t *b) {
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

bool LinearFrameBuffer::setResolution(const Io::File &lfbFile, const uint16_t x, const uint16_t y, const uint8_t bpp) {
    return lfbFile.controlFile(SET_RESOLUTION, Util::Array<uint32_t>({x, y, bpp}));
}

bool LinearFrameBuffer::setResolution(const Io::File &lfbFile, const String &resolutionString) {
    const auto split1 = resolutionString.split("x");
    const auto split2 = split1[1].split("@");

    const auto x = String::parseNumber<uint16_t>(split1[0]);
    const auto y = String::parseNumber<uint16_t>(split2[0]);
    const auto bpp = String::parseNumber<uint8_t>(split2[1]);

    return setResolution(lfbFile, x, y, bpp);
}

Color LinearFrameBuffer::readPixel(const uint16_t x, const uint16_t y) const {
    if (x > resolutionX - 1 || y > resolutionY - 1) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "LinearFrameBuffer: Trying to read a pixel out of bounds!");
    }

    const auto *address = reinterpret_cast<const uint32_t*>(buffer.add(x * ((colorDepth + 7) / 8) + y * pitch).get());
    return Color::fromRGB(*address, colorDepth);
}

void LinearFrameBuffer::drawPixel(const int32_t x, const int32_t y, const Color &color) const {
    // Pixels outside the visible area won't be drawn
    if (x < 0 || x >= resolutionX || y < 0 || y >= resolutionY) {
        return;
    }

    // Invisible pixels won't be drawn
    if (color.getAlpha() == 0) {
        return;
    }

    // Blend if necessary
    const Color toWrite = color.getAlpha() < 255 ? readPixel(x, y).blend(color) : color;

    switch (colorDepth) {
        case 32:
        {
            const auto offset = x + y * (pitch / 4);
            auto *pixelBuffer = reinterpret_cast<uint32_t*>(buffer.get());

            pixelBuffer[offset] = toWrite.getRGB32();

            return;
        }

        case 24:
        {
            const auto rgbColor = toWrite.getRGB24();
            const auto offset = x * 3 + y * pitch;
            auto *pixelBuffer = reinterpret_cast<uint8_t*>(buffer.get());

            pixelBuffer[offset] = rgbColor & 0xff;
            pixelBuffer[offset + 1] = (rgbColor >> 8) & 0xff;
            pixelBuffer[offset + 2] = (rgbColor >> 16) & 0xff;

            return;
        }

        case 16:
        {
            const auto offset = x + y * (pitch / 2);
            auto *pixelBuffer = reinterpret_cast<uint16_t*>(buffer.get());

            pixelBuffer[offset] = toWrite.getRGB16();

            return;
        }

        case 15:
        {
            const auto offset = x + y * (pitch / 2);
            auto *pixelBuffer = reinterpret_cast<uint16_t*>(buffer.get());

            pixelBuffer[offset] = toWrite.getRGB15();

            return;
        }

        case 2: // CGA 4-color mode
        {
            const auto offset = x / (8 / colorDepth) + y / (4 / colorDepth) * pitch + (y % 2) * 0x2000;
            const auto pixelValue = toWrite.getRGB2();
            auto *pixelBuffer = reinterpret_cast<uint8_t*>(buffer.add(offset).get());

            const auto pos = x & 3;
            const auto shift = (3 - pos) * 2;
            const auto mask = static_cast<uint8_t>(0x3u << shift);
            *pixelBuffer = static_cast<uint8_t>((*pixelBuffer & ~mask) | ((pixelValue & 0x3u) << shift));

            return;
        }

        case 1: // CGA 2-color mode
        {
            const auto offset = x / (8 / colorDepth) + y / (4 / colorDepth) * pitch + (y % 2) * 0x2000;
            const auto pixelValue = toWrite.getRGB1();
            auto *pixelBuffer = reinterpret_cast<uint8_t*>(buffer.add(offset).get());

            const auto pos = x & 7;
            const auto shift = 7 - pos;
            const auto mask = static_cast<uint8_t>(1u << shift);
            *pixelBuffer = static_cast<uint8_t>((*pixelBuffer & ~mask) | ((pixelValue & 0x1u) << shift));

            return;
        }

        default:
            Panic::fire(Panic::UNSUPPORTED_OPERATION, "LinearFrameBuffer: Unsupported color depth!");
    }
}

void LinearFrameBuffer::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color &color) const {
    if ((x1 < 0 && x2 < 0) || (x1 > resolutionX && x2 >= resolutionX)) {
        return;
    }
    if ((y1 < 0 && y2 < 0) || (y1 > resolutionY && y2 >= resolutionY)) {
        return;
    }

    if (x1 < 0) {
        x1 = 0;
    }
    if (x1 >= resolutionX) {
        x1 = resolutionX - 1;
    }

    if (y1 < 0) {
        y1 = 0;
    }
    if (y1 >= resolutionY) {
        y1 = resolutionY - 1;
    }

    if (x2 < 0) {
        x2 = 0;
    }
    if (x2 >= resolutionX) {
        x2 = resolutionX - 1;
    }

    if (y2 < 0) {
        y2 = 0;
    }
    if (y2 >= resolutionY) {
        y2 = resolutionY - 1;
    }

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

void LinearFrameBuffer::drawRectangle(const int32_t x, const int32_t y, const uint16_t width, const uint16_t height,
    const Color &color) const
{
    drawLine(x, y, x + width - 1, y, color);
    drawLine(x, y + height - 1, x + width - 1, y + height - 1, color);
    drawLine(x, y, x, y + height - 1, color);
    drawLine(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void LinearFrameBuffer::drawSquare(const int32_t x, const int32_t y, const uint16_t size, const Color &color) const {
    drawRectangle(x, y, size, size, color);
}

void LinearFrameBuffer::fillRectangle(const int32_t x, const int32_t y, const uint16_t width, const uint16_t height,
    const Color &color) const
{
    const auto endX = x + width - 1;
    const auto endY = y + height -1;

    for (auto i = y; i <= endY; i++) {
        drawLine(x, i, endX, i, color);
    }
}

void LinearFrameBuffer::fillSquare(const int32_t x, const int32_t y, const uint16_t size, const Color &color) const {
    fillRectangle(x, y, size, size, color);
}

void LinearFrameBuffer::drawCircle(const int32_t x, const int32_t y, const uint16_t radius, const Color &color) const {
    for (int32_t dy = -radius; dy <= radius; dy++) {
        const auto dx = static_cast<int32_t>(Math::sqrt(static_cast<float>(radius * radius - dy * dy)));

        drawPixel(x + dx, y + dy, color);
        drawPixel(x - dx, y + dy, color);
    }
}

void LinearFrameBuffer::fillCircle(const int32_t x, const int32_t y, const uint16_t radius, const Color &color) const {
    for (int32_t dy = -radius; dy <= radius; dy++) {
        const auto dxLimit = static_cast<int32_t>(Math::sqrt(static_cast<float>(radius * radius - dy * dy)));

        for (int32_t dx = -dxLimit; dx <= dxLimit; dx++) {
            drawPixel(x + dx, y + dy, color);
        }
    }
}

void LinearFrameBuffer::drawChar(const Font &font, const int32_t x, const int32_t y, const char c,
    const Color &fgColor, const Color &bgColor) const
{
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(c));
}

void LinearFrameBuffer::drawString(const Font &font, int32_t x, const int32_t y, const char *string,
    const Color &fgColor, const Color &bgColor) const
{
    for (uint32_t i = 0; string[i] >= 0x20 && string[i] <= 0x7e; ++i) {
        drawChar(font, x, y, string[i], fgColor, bgColor);
        x += font.getCharWidth();
    }
}

void LinearFrameBuffer::drawString(const Font &font, const int32_t x, const int32_t y, const String &string,
    const Color &fgColor, const Color &bgColor) const
{
    drawString(font, x, y, static_cast<const char*>(string), fgColor, bgColor);
}

void LinearFrameBuffer::drawImage(const Image &image, const int32_t x, const int32_t y) const {
    const auto pixelBuffer = image.getPixelBuffer();
    const auto width = image.getWidth();
    const auto height = image.getHeight();

    for (uint16_t offsetY = 0; offsetY < height; ++offsetY) {
        for (uint16_t offsetX = 0; offsetX < width; ++offsetX) {
            drawPixel(x + offsetX, y + offsetY, pixelBuffer[offsetY * width + offsetX]);
        }
    }
}

void LinearFrameBuffer::scrollUp(const uint16_t lineCount, const bool clearBelow) const {
    // Move screen buffer upwards by the given number of lines
    const auto source = buffer.add(pitch * lineCount);
    buffer.copyRange(source, pitch * (resolutionY - lineCount));

    // Clear lower part of the screen
    if (clearBelow) {
        const auto clear = buffer.add(pitch * (resolutionY - lineCount));
        clear.setRange(0, pitch * lineCount);
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

void LinearFrameBuffer::drawLineMajorAxis(int32_t x, int32_t y, const int8_t xMovement, const int8_t yMovement,
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

void LinearFrameBuffer::drawLineSingleAxis(int32_t x, int32_t y, const int8_t movement, int32_t dx,
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

void LinearFrameBuffer::drawMonoBitmap(const int32_t x, const int32_t y, const uint16_t width, const uint16_t height,
    const Color &fgColor, const Color &bgColor, const uint8_t *bitmap) const
{
    // Each bit in the bitmap represents a pixel (1 = fgColor, 0 = bgColor)
    // and is used as index into this array for drawing (avoids branching)
    const Color *colors[] = { &bgColor, &fgColor };
    const uint16_t widthInBytes = (width + 7) / 8;

    for (uint16_t offsetY = 0; offsetY < height; ++offsetY) {
        auto posX = x;
        const auto posY = y + offsetY;

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

}
}