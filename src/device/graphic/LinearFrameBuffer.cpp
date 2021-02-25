/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

namespace Device {

static uint32_t diff(uint32_t a, uint32_t b) {
    return a > b ? a - b : b - a;
}

LinearFrameBuffer::LinearFrameBuffer(void *address, uint16_t resolutionX, uint16_t resolutionY, uint8_t bitsPerPixel, uint16_t pitch) :
        resolutionX(resolutionX), resolutionY(resolutionY), bitsPerPixel(bitsPerPixel), pitch(pitch), hardwareBuffer(static_cast<uint8_t *>(address)) {

}

LinearFrameBuffer::LfbResolution LinearFrameBuffer::findBestResolution(uint16_t resX, uint16_t resY, uint8_t depth) {
    Util::Array<LfbResolution> resolutions = getLfbResolutions();
    Util::ArrayList<LfbResolution> candidates;

    uint32_t bestDiff = 0xffffffff;
    LfbResolution bestRes{};

    // Find a resolution with the closest resX and resY to the desired resX and resY.
    for (const LfbResolution &currentRes : resolutions) {
        uint32_t currentDiff = diff(resX, currentRes.resX) + diff(resY, currentRes.resY);

        if (currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestRes = currentRes;
        }
    }

    // Put all resolutions with the same resX and resY as bestRes in the candidates-list.
    for (const LfbResolution &currentRes : resolutions) {
        if (currentRes.resX == bestRes.resX && currentRes.resY == bestRes.resY) {
            candidates.add(currentRes);
        }
    }

    // Find the resolution with the closest depth to the desired depth.
    bestDiff = 0xffffffff;
    for (const LfbResolution &currentRes : candidates) {
        uint32_t currentDiff = diff(depth, currentRes.depth);

        if (currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestRes = currentRes;
        }
    }

    return bestRes;
}

bool LinearFrameBuffer::init(uint16_t resX, uint16_t resY, uint8_t depth) {
    LfbResolution res = findBestResolution(resX, resY, depth);

    disableDoubleBuffering();

    bool ret = setResolution(res);

    if (ret) {
        resolutionX = res.resX;
        resolutionY = res.resY;
        bitsPerPixel = res.depth;

        clear();
    }

    return ret;
}

bool LinearFrameBuffer::setResolution(LinearFrameBuffer::LfbResolution resolution) {
    return resolution.resX == resolutionX && resolution.resY == resolutionY && resolution.depth == bitsPerPixel;
}

Util::String LinearFrameBuffer::getName() {
    return NAME;
}

bool LinearFrameBuffer::isAvailable() {
    return false;
}

Util::Array<LinearFrameBuffer::LfbResolution> LinearFrameBuffer::getLfbResolutions() {
    Util::Array<LfbResolution> ret(1);

    ret[0] = LfbResolution{resolutionX, resolutionY, bitsPerPixel, 0};

    return ret;
}

Util::String LinearFrameBuffer::getVendorName() {
    return VENDOR_NAME;
}

Util::String LinearFrameBuffer::getDeviceName() {
    return DEVICE_NAME;
}

uint32_t LinearFrameBuffer::getVideoMemorySize() {
    return resolutionX * resolutionY * (bitsPerPixel / 8u);
}

void LinearFrameBuffer::drawPixel(uint16_t x, uint16_t y, Util::Color color) {
    //Pixels outside the visible area won't be drawn
    if (x > resolutionX - 1 || y > resolutionY - 1) return;

    auto bpp = static_cast<uint8_t>(this->bitsPerPixel == 15 ? 16 : this->bitsPerPixel);

    if (color.getAlpha() < 255) {
        if (color.getAlpha() == 0) {
            return;
        }

        Util::Color currentColor;
        readPixel(x, y, currentColor);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(this->bitsPerPixel);

    uint8_t *base = doubleBuffered ? doubleBuffer : hardwareBuffer;

    //Calculate pixel offset
    uint8_t *ptr = base + (x * (bpp / 8)) + y * pitch;

    //Write color to pixel offset
    for (uint32_t i = 0; i < (bpp / 8); i++) {
        ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
    }
}

void LinearFrameBuffer::readPixel(uint16_t x, uint16_t y, Util::Color &color) {
    if (x > resolutionX - 1 || y > resolutionY - 1) {
        return;
    }

    auto bpp = static_cast<uint8_t>(this->bitsPerPixel == 15 ? 16 : this->bitsPerPixel);
    uint8_t *base = doubleBuffered ? doubleBuffer : hardwareBuffer;
    uint8_t *ptr = base + (x * (bpp / 8)) + y * pitch;

    color.setRGB(*((uint32_t *) ptr), this->bitsPerPixel);
}

void LinearFrameBuffer::drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Util::Color fgColor,
                                       Util::Color bgColor, uint8_t *bitmap) {
    auto widthInBytes = static_cast<uint16_t>(width / 8 + ((width % 8 != 0) ? 1 : 0));

    for (uint16_t yoff = 0; yoff < height; ++yoff) {
        uint16_t xpos = x;
        uint16_t ypos = y + yoff;

        for (uint16_t xb = 0; xb < widthInBytes; ++xb) {
            for (int8_t src = 7; src >= 0; --src) {
                if ((1 << src) & *bitmap)
                    drawPixel(xpos, ypos, fgColor);
                else
                    drawPixel(xpos, ypos, bgColor);
                xpos++;
            }
            bitmap++;
        }
    }
}

void LinearFrameBuffer::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Util::Color color) {
    int16_t dx, dy, d, incE, incNE, slope;
    uint16_t x, y;

    // Punkt tauschen
    if (x1 > x2) {
        drawLine(x2, y2, x1, y1, color);
        return;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    if (dy < 0) {
        slope = -1;
        dy = -dy;
    } else {
        slope = 1;
    }

    // Vertikale Linie zeichnen
    if (dx == 0) {
        for (int i = 0; i <= dy; i++) {
            drawPixel(x1, y1, color);
            y1 += slope;
        }
    } else {
        d = static_cast<int16_t>(2 * dy - dx);
        incE = static_cast<int16_t>(2 * dy);
        incNE = static_cast<int16_t>(2 * (dy - dx));
        y = y1;
        for (x = x1; x <= x2; x++) {
            drawPixel(x, y, color);
            if (d > 0) {
                d += incNE;
                y += slope;
            } else {
                d += incE;
            }
        }
    }
}

void LinearFrameBuffer::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Util::Color color) {
    drawLine(x, y, x + width, y, color);
    drawLine(x + width, y, x + width, y + height, color);
    drawLine(x + width, y + height, x, y + height, color);
    drawLine(x, y + height, x, y, color);
}

void LinearFrameBuffer::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Util::Color color) {
    for (uint16_t i = y; i <= y + height; i++) {
        drawLine(x, i, x + width, i, color);
    }
}

void LinearFrameBuffer::fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, Util::Color color) {
    int16_t x = radius;
    int16_t y = 0;
    auto xChange = static_cast<int16_t>(1 - (radius << 1));
    int16_t yChange = 0;
    int16_t radiusError = 0;

    while (x >= y) {
        for (uint16_t i = x0 - x; i <= x0 + x; i++) {
            drawPixel(i, y0 + y, color);
            drawPixel(i, y0 - y, color);
        }
        for (uint16_t i = x0 - y; i <= x0 + y; i++) {
            drawPixel(i, y0 + x, color);
            drawPixel(i, y0 - x, color);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0) {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

void
LinearFrameBuffer::drawChar(const Util::Font &font, uint16_t x, uint16_t y, char c, Util::Color fgColor, Util::Color bgColor) {
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(' '));
    drawMonoBitmap(x, y, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(c));
}

void LinearFrameBuffer::drawString(const Util::Font &font, uint16_t x, uint16_t y, const char *s, Util::Color fgColor,
                                   Util::Color bgColor) {
    for (uint32_t i = 0; s[i] != 0; ++i) {
        drawChar(font, x, y, s[i], fgColor, bgColor);
        x += font.getCharWidth();
    }
}

void LinearFrameBuffer::drawSprite(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, int32_t *data) {
    uint16_t yh = yPos + height;
    uint16_t xw = xPos + width;

    for (uint16_t y = yPos; y < yh; y++) {
        for (uint16_t x = xPos; x < xw; x++) {

            if (x >= resolutionX || y >= resolutionY) {
                continue;
            }

            auto pixel = static_cast<uint32_t>(data[(x - xPos) + (y - yPos) * width]);

            auto alpha = static_cast<uint8_t>(pixel >> 24);
            auto blue = static_cast<uint8_t>((pixel & 16711680) >> 16);
            auto green = static_cast<uint8_t>((pixel & 65280) >> 8);
            auto red = static_cast<uint8_t>(pixel & 255);

            Util::Color color = Util::Color(red, green, blue, alpha);
            drawPixel(x, y, color);
        }
    }
}

void LinearFrameBuffer::clear() {
    auto *buf = reinterpret_cast<uint64_t *>(doubleBuffered ? doubleBuffer : hardwareBuffer);

    for (uint32_t i = 0; i < (resolutionX * resolutionY * (bitsPerPixel / 8u)) / 8u; i++) {
        buf[i] = 0;
    }

    for (uint32_t i = 0; i < (resolutionX * resolutionY * (bitsPerPixel / 8u)) % 8u; i++) {
        buf[(resolutionX * resolutionY) / 8 + i] = 0;
    }
}

void LinearFrameBuffer::placeLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Util::Color color) {
    auto posX1 = static_cast<uint16_t>((resolutionX * x1) / 100);
    auto posY1 = static_cast<uint16_t>((resolutionY * y1) / 100);
    auto posX2 = static_cast<uint16_t>((resolutionX * x2) / 100);
    auto posY2 = static_cast<uint16_t>((resolutionY * y2) / 100);

    drawLine(posX1, posY1, posX2, posY2, color);
}

void LinearFrameBuffer::placeRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Util::Color color) {
    auto newWidth = static_cast<uint16_t>((resolutionX * width) / 100);
    auto newHeight = static_cast<uint16_t>((resolutionY * height) / 100);
    auto posX = static_cast<uint16_t>((resolutionX * x) / 100 - width / 2);
    auto posY = static_cast<uint16_t>((resolutionY * y) / 100 - height / 2);

    drawRect(posX, posY, newWidth, newHeight, color);
}

void
LinearFrameBuffer::placeFilledRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Util::Color color) {
    auto newWidth = static_cast<uint16_t>((resolutionX * width) / 100);
    auto newHeight = static_cast<uint16_t>((resolutionY * height) / 100);
    auto posX = static_cast<uint16_t>((resolutionX * x) / 100);
    auto posY = static_cast<uint16_t>((resolutionY * y) / 100);

    fillRect(posX, posY, newWidth, newHeight, color);
}

void LinearFrameBuffer::placeFilledCircle(uint16_t x, uint16_t y, uint16_t radius, Util::Color color) {
    auto posX = static_cast<uint16_t>((resolutionX * x) / 100);
    auto posY = static_cast<uint16_t>((resolutionY * y) / 100);
    auto newRadius = static_cast<uint16_t>(((resolutionX > resolutionY ? resolutionY : resolutionX) * radius) / 100);

    fillCircle(posX, posY, newRadius, color);
}

void
LinearFrameBuffer::placeString(Util::Font &font, uint16_t x, uint16_t y, const char *s, Util::Color fgColor,
                               Util::Color bgColor) {
    uint8_t charWidth = font.getCharWidth();
    uint8_t charHeight = font.getCharHeight();

    auto posX = static_cast<uint16_t>((resolutionX * x) / 100 - (charWidth * strlen(s) / 2));
    auto posY = static_cast<uint16_t>((resolutionY * y) / 100 - (charHeight / 2));

    for (uint32_t i = 0; s[i] != '\0'; i++) {
        drawMonoBitmap(posX, posY, font.getCharWidth(), font.getCharHeight(), fgColor, bgColor, font.getChar(s[i]));
        posX += font.getCharWidth();
    }
}

void LinearFrameBuffer::placeSprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int32_t *data) {
    auto posX = static_cast<uint16_t>((resolutionX * x) / 100 - width / 2);
    auto posY = static_cast<uint16_t>((resolutionY * y) / 100 - height / 2);

    drawSprite(posX, posY, width, height, data);
}

uint16_t LinearFrameBuffer::getResX() const {
    return resolutionX;
}

uint16_t LinearFrameBuffer::getResY() const {
    return resolutionY;
}

uint8_t LinearFrameBuffer::getDepth() const {
    return bitsPerPixel;
}

void LinearFrameBuffer::enableDoubleBuffering() {
    reallocBuffer();

    doubleBuffered = true;
    clear();
}

void LinearFrameBuffer::disableDoubleBuffering() {
    doubleBuffered = false;
}

bool LinearFrameBuffer::isDoubleBuffered() {
    return doubleBuffered;
}

void LinearFrameBuffer::show() {
    if (!doubleBuffered) {
        return;
    }

    auto bpp = static_cast<uint8_t>(this->bitsPerPixel == 15 ? 16 : this->bitsPerPixel);

    uint32_t i;
    auto *dest = reinterpret_cast<uint64_t *>(hardwareBuffer);
    auto *src = reinterpret_cast<uint64_t *>(doubleBuffer);
    uint64_t end = (resolutionX * resolutionY * (bpp / 8)) / sizeof(uint64_t);

    for (i = 0; i < end; i++) {
        dest[i] = src[i];
        src[i] = 0;
    }
}

void LinearFrameBuffer::reallocBuffer() {
    if (doubleBuffer != nullptr) {
        delete[] doubleBuffer;
        doubleBuffer = nullptr;
    }

    auto bpp = static_cast<uint8_t>(this->bitsPerPixel == 15 ? 16 : this->bitsPerPixel);
    doubleBuffer = new uint8_t[resolutionX * resolutionY * (bpp / 8)];
}

}