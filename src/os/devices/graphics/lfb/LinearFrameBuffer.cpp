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

#include <kernel/memory/SystemManagement.h>
#include "LinearFrameBuffer.h"

static uint32_t diff(uint32_t a, uint32_t b) {
    return a > b ? a - b : b - a;
}

LinearFrameBuffer::LinearFrameBuffer(void *address, uint16_t xres, uint16_t yres, uint8_t bpp, uint16_t pitch) :
        xres(xres), yres(yres), bpp(bpp), pitch(pitch), hardwareBuffer(static_cast<uint8_t *>(address)) {

}

LinearFrameBuffer::LfbResolution LinearFrameBuffer::findBestResolution(uint16_t resX, uint16_t resY, uint8_t depth) {
    Util::Array<LfbResolution> resolutions = getLfbResolutions();
    Util::ArrayList<LfbResolution> candidates;

    uint32_t bestDiff = 0xffffffff;
    LfbResolution bestRes{};

    // Find a resolution with the closest resX and resY to the desired resX and resY.
    for(const LfbResolution &currentRes : resolutions) {
        uint32_t currentDiff = diff(resX, currentRes.resX) + diff(resY, currentRes.resY);

        if(currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestRes = currentRes;
        }
    }

    // Put all resolutions with the same resX and resY as bestRes in the candidates-list.
    for(const LfbResolution &currentRes : resolutions) {
        if(currentRes.resX == bestRes.resX && currentRes.resY == bestRes.resY) {
            candidates.add(currentRes);
        }
    }

    // Find the resolution with the closest depth to the desired depth.
    bestDiff = 0xffffffff;
    for(const LfbResolution &currentRes : candidates) {
        uint32_t currentDiff = diff(depth, currentRes.depth);

        if(currentDiff < bestDiff) {
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

    if(ret) {
        xres = res.resX;
        yres = res.resY;
        bpp = res.depth;

        clear();
    }

    return ret;
}

bool LinearFrameBuffer::setResolution(LinearFrameBuffer::LfbResolution resolution) {
    return resolution.resX == xres && resolution.resY == yres && resolution.depth == bpp;
}

String LinearFrameBuffer::getName() {
    return NAME;
}

bool LinearFrameBuffer::isAvailable() {
    return false;
}

Util::Array<LinearFrameBuffer::LfbResolution> LinearFrameBuffer::getLfbResolutions() {
    Util::Array<LfbResolution> ret(1);

    ret[0] = LfbResolution{xres, yres, bpp, 0};

    return ret;
}

String LinearFrameBuffer::getVendorName() {
    return VENDOR_NAME;
}

String LinearFrameBuffer::getDeviceName() {
    return DEVICE_NAME;
}

uint32_t LinearFrameBuffer::getVideoMemorySize() {
    return xres * yres * (bpp / 8u);
}

void LinearFrameBuffer::drawPixel(uint16_t x, uint16_t y, Color color) {
    //Pixels outside the visible area won't be drawn
    if(x > xres - 1 || y > yres - 1) return;

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);

    if(color.getAlpha() < 255) {
        if(color.getAlpha() == 0) {
            return;
        }

        Color currentColor;
        readPixel(x, y, currentColor);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(this->bpp);

    uint8_t *base = doubleBuffered ? doubleBuffer : hardwareBuffer;

    //Calculate pixel offset
    uint8_t *ptr = base + (x * (bpp / 8)) + y * pitch;

    //Write color to pixel offset
    for(uint32_t i = 0; i < (bpp / 8); i++) {
        ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
    }
}

void LinearFrameBuffer::readPixel(uint16_t x, uint16_t y, Color &color) {
    if (x > xres - 1 || y > yres - 1) {
        return;
    }

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);
    uint8_t *base = doubleBuffered ? doubleBuffer : hardwareBuffer;
    uint8_t *ptr = base + (x * (bpp / 8)) + y * pitch;

    color.setRGB(*((uint32_t *)ptr), this->bpp);
}

void LinearFrameBuffer::drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color fgColor, Color bgColor, uint8_t *bitmap) {
    auto widthInBytes = static_cast<uint16_t>(width / 8 + ((width % 8 != 0) ? 1 : 0));

    for(uint16_t yoff = 0; yoff < height; ++yoff) {
        uint16_t xpos = x;
        uint16_t ypos = y + yoff;

        for(uint16_t xb = 0; xb < widthInBytes; ++xb) {
            for(int8_t src = 7; src >= 0; --src) {
                if((1 << src) & *bitmap)
                    drawPixel(xpos, ypos, fgColor);
                else
                    drawPixel(xpos, ypos, bgColor);
                xpos++;
            }
            bitmap++;
        }
    }
}

void LinearFrameBuffer::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color) {
    int16_t dx, dy, d, incE, incNE, slope;
    uint16_t x, y;

    // Punkt tauschen
    if(x1 > x2) {
        drawLine(x2, y2, x1, y1, color);
        return;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    if (dy < 0) {
        slope  = -1;
        dy = -dy;
    } else {
        slope  = 1;
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
                y +=slope;
            } else {
                d += incE;
            }
        }
    }
}

void LinearFrameBuffer::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color) {
    drawLine(x, y, x + width, y, color);
    drawLine(x + width, y, x + width, y + height, color);
    drawLine(x + width, y + height, x, y + height, color);
    drawLine(x, y + height, x, y, color);
}

void LinearFrameBuffer::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color) {
    for(uint16_t i = y; i <= y + height; i++) {
        drawLine(x, i, x + width, i, color);
    }
}

void LinearFrameBuffer::fillCircle(uint16_t x0, uint16_t y0, uint16_t r, Color color) {
    int16_t x = r;
    int16_t y = 0;
    auto xChange = static_cast<int16_t>(1 - (r << 1));
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

void LinearFrameBuffer::drawChar(Font &fnt, uint16_t x, uint16_t y, char c, Color fgColor, Color bgColor) {
    drawMonoBitmap(x, y, fnt.get_char_width(), fnt.get_char_height(), fgColor, bgColor, fnt.getChar(' '));
    drawMonoBitmap(x, y, fnt.get_char_width(), fnt.get_char_height(), fgColor, bgColor, fnt.getChar(c));
}

void LinearFrameBuffer::drawString(Font &fnt, uint16_t x, uint16_t y, const char *s, Color fgColor, Color bgColor) {
    for(uint32_t i = 0; s[i] != 0; ++i) {
        drawChar(fnt, x, y, s[i], fgColor, bgColor);
        x += fnt.get_char_width();
    }
}

void LinearFrameBuffer::drawSprite(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, int32_t *data) {
    uint16_t yh = yPos + height;
    uint16_t xw = xPos + width;

    for(uint16_t y = yPos; y < yh; y++){
        for(uint16_t x = xPos; x < xw; x++){

            if (x >= xres || y >= yres){
                continue;
            }

            auto pixel = static_cast<uint32_t>(data[(x - xPos) + (y - yPos) * width]);

            auto alpha = static_cast<uint8_t>(pixel >> 24);
            auto blue = static_cast<uint8_t>((pixel & 16711680) >> 16);
            auto green = static_cast<uint8_t>((pixel & 65280) >> 8);
            auto red = static_cast<uint8_t>(pixel & 255);

            Color color = Color(red, green, blue, alpha);
            drawPixel(x, y, color);
        }
    }
}

void LinearFrameBuffer::clear() {
    for(uint16_t x = 0; x < xres; x++){
        for(uint16_t y = 0; y < yres; y++){
            drawPixel(x, y, Colors::BLACK);
        }
    }
}

void LinearFrameBuffer::placeLine(uint16_t x1_p, uint16_t y1_p, uint16_t x2_p, uint16_t y2_p, Color color) {
    auto y1 = static_cast<uint16_t>((yres * y1_p) / 100);
    auto x1 = static_cast<uint16_t>((xres * x1_p) / 100);
    auto y2 = static_cast<uint16_t>((yres * y2_p) / 100);
    auto x2 = static_cast<uint16_t>((xres * x2_p) / 100);

    drawLine(x1, y1, x2, y2, color);
}

void LinearFrameBuffer::placeRect(uint16_t x_p, uint16_t y_p, uint16_t width_p, uint16_t height_p, Color color) {
    auto width = static_cast<uint16_t>((xres * width_p) / 100);
    auto height = static_cast<uint16_t>((yres * height_p) / 100);
    auto y = static_cast<uint16_t>((yres * y_p) / 100 - height / 2);
    auto x = static_cast<uint16_t>((xres * x_p) / 100 - width / 2);

    drawRect(x, y, width, height, color);
}

void LinearFrameBuffer::placeFilledRect(uint16_t x_p, uint16_t y_p, uint16_t width_p, uint16_t height_p, Color color) {
    auto width = static_cast<uint16_t>((xres * width_p) / 100);
    auto height = static_cast<uint16_t>((yres * height_p) / 100);
    auto y = static_cast<uint16_t>((yres * y_p) / 100);
    auto x = static_cast<uint16_t>((xres * x_p) / 100);

    fillRect(x, y, width, height, color);
}

void LinearFrameBuffer::placeFilledCircle(uint16_t x_p, uint16_t y_p, uint16_t r_p, Color color) {
    auto y = static_cast<uint16_t>((yres * y_p) / 100);
    auto x = static_cast<uint16_t>((xres * x_p) / 100);
    auto r = static_cast<uint16_t>(((xres > yres ? yres : xres) * r_p) / 100);

    fillCircle(x, y, r, color);
}

void
LinearFrameBuffer::placeString(Font &fnt, uint16_t x_p, uint16_t y_p, const char *s, Color fgColor, Color bgColor) {
    uint8_t charWidth =  fnt.get_char_width();
    uint8_t charHeight =  fnt.get_char_height();

    auto y = static_cast<uint16_t>((yres * y_p) / 100 - (charHeight / 2));
    auto x = static_cast<uint16_t>((xres * x_p) / 100 - (charWidth * strlen(s) / 2));

    for(uint32_t i = 0; s[i] != '\0'; i++){
        drawMonoBitmap(x, y, fnt.get_char_width(), fnt.get_char_height(), fgColor, bgColor, fnt.getChar(s[i]));
        x += fnt.get_char_width();
    }
}

void LinearFrameBuffer::placeSprite(uint16_t x_p, uint16_t y_p, uint16_t width, uint16_t height, int32_t *data) {
    auto x = static_cast<uint16_t>((xres * x_p) / 100 - width / 2);
    auto y = static_cast<uint16_t>((yres * y_p) / 100 - height / 2);

    drawSprite(x, y, width, height, data);
}

uint16_t LinearFrameBuffer::getResX() {
    return xres;
}

uint16_t LinearFrameBuffer::getResY() {
    return yres;
}

uint8_t LinearFrameBuffer::getDepth() {
    return bpp;
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

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);

    uint32_t i;
    auto *dest = reinterpret_cast<uint64_t *>(hardwareBuffer);
    auto *src =  reinterpret_cast<uint64_t *>(doubleBuffer);
    uint64_t end = (xres * yres * (bpp / 8)) / sizeof(uint64_t);

    for (i = 0; i < end; i++) {
        dest[i] = src[i];
        src[i] = 0;
    }
}

void LinearFrameBuffer::reallocBuffer() {
    if(doubleBuffer != nullptr) {
        delete[] doubleBuffer;
        doubleBuffer = nullptr;
    }

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);
    doubleBuffer = new uint8_t[xres * yres * (bpp / 8)];
}
