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

#include <devices/misc/Bios.h>
#include <kernel/memory/SystemManagement.h>
#include "LfbText.h"

LfbText::LfbText(void *address, uint16_t xres, uint16_t yres, uint8_t bpp, uint16_t pitch) :
        LinearFrameBuffer(address, xres, yres, bpp, pitch), font(&std_font_8x16) {

}

String LfbText::getName() {
    return NAME;
}

bool LfbText::isAvailable() {
    return LinearFrameBuffer::isAvailable();
}

bool LfbText::setResolution(TextResolution resolution) {
    uint8_t fontWidth = font->get_char_width();
    uint8_t fontHeight = font->get_char_height();

    if(shadowBuf != nullptr) {
        delete[] shadowBuf;
        shadowBuf = nullptr;
    }

    if(LinearFrameBuffer::init(resolution.columns * fontWidth, resolution.rows * fontHeight, resolution.depth)) {
        this->columns = LinearFrameBuffer::xres / fontWidth;
        this->rows = LinearFrameBuffer::yres / fontHeight;
        this->depth = LinearFrameBuffer::bpp;

        auto bpp = static_cast<uint8_t>(this->depth == 15 ? 16 : this->depth);
        auto bufSize = static_cast<uint32_t>(LinearFrameBuffer::xres * LinearFrameBuffer::yres * (bpp / 8));

        shadowBuf = new uint8_t[bufSize];
        memset(shadowBuf, 0, bufSize);

        return true;
    }

    return false;
}

Util::Array<TextDriver::TextResolution> LfbText::getTextResolutions() {
    if(!resolutions.isEmpty()) {
        return resolutions.toArray();
    }

    Util::Array<LinearFrameBuffer::LfbResolution> lfbResolutions = LinearFrameBuffer::getLfbResolutions();

    uint16_t fontWidth = font->get_char_width();
    uint16_t fontHeight = font->get_char_height();

    for(const LinearFrameBuffer::LfbResolution &currentLfbRes : lfbResolutions) {

        auto *currentRes = new TextResolution();
        *currentRes = {static_cast<uint16_t >(currentLfbRes.resX / fontWidth), static_cast<uint16_t >(currentLfbRes.resY / fontHeight), currentLfbRes.depth, currentLfbRes.modeNumber};
        resolutions.add(*currentRes);
    }

    return resolutions.toArray();
}

String LfbText::getVendorName() {
    return LinearFrameBuffer::getVendorName();
}

String LfbText::getDeviceName() {
    return LinearFrameBuffer::getDeviceName();
}

uint32_t LfbText::getVideoMemorySize() {
    return LinearFrameBuffer::getVideoMemorySize();
}

void LfbText::drawPixel(uint16_t x, uint16_t y, Color color) {
    //Pixels outside the visible area won't be drawn
    if(x > xres || y > yres) return;

    auto bpp = static_cast<uint8_t>(this->bpp == 15 ? 16 : this->bpp);

    if(color.getAlpha() < 255) {
        if(color.getAlpha() == 0) {
            return;
        }

        Color currentColor;
        LinearFrameBuffer::readPixel(x, y, currentColor);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(this->bpp);

    //Calculate pixel offset
    uint8_t *ptr = LinearFrameBuffer::hardwareBuffer + (x * (bpp / 8)) + y * pitch;

    //Write color to pixel offset
    for(uint32_t i = 0; i < (bpp / 8); i++) {
        ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
    }

    if(shadowBuf != nullptr) {
        //Calculate pixel offset
        ptr = shadowBuf + (x * (bpp / 8)) + y * pitch;

        //Write color to pixel offset
        for(uint32_t i = 0; i < (bpp / 8); i++) {
            ptr[i] = static_cast<uint8_t>((rgbColor >> (i * 8)) & 0xff);
        }
    }
}

void LfbText::setpos(uint16_t x, uint16_t y) {
    pos = x + columns * y;
}

void LfbText::getpos (uint16_t &x, uint16_t &y) {
    x = pos % columns;
    y = pos / columns;
}

void LfbText::show(uint16_t x, uint16_t y, char c, Color fgColor, Color bgColor) {
    uint16_t fontWidth = font->get_char_width();
    uint16_t fontHeight = font->get_char_height();

    LfbText::drawChar(*font, x * fontWidth, y * fontHeight, c, fgColor, bgColor);
}

void LfbText::putc(char c, Color fgColor, Color bgColor) {
    puts(&c, 1, fgColor, bgColor);
}

void LfbText::puts(const char *s, uint32_t n, Color fgColor, Color bgColor) {
    uint16_t x, y;
    getpos(x, y);
    uint16_t pos = x + columns * y;

    for (uint32_t i = 0; i < n; i++) {

        if (s[i] == '\0') {
            break;
        }

        if(s[i] == '\n') {
            show(pos % columns, pos / columns, ' ', fgColor, bgColor);
            pos += columns - (pos % columns);
        } else {
            show(pos % columns, pos / columns, s[i], fgColor, bgColor);
            pos++;
        }

        if(pos >= columns * rows) {
            scrollup();
            pos = rows * columns - columns;
        }
    }

    setpos(pos % columns, pos / columns);
}

void LfbText::scrollup () {

    uint16_t fontWidth = font->get_char_width();
    uint16_t fontHeight = font->get_char_height();

    auto bpp = static_cast<uint8_t>(LfbText::bpp == 15 ? 16 : LfbText::bpp);
    uint64_t *src, *dest;
    uint64_t end;

    // Move screen-buffer upwards by one line
    src = (uint64_t *) (shadowBuf + (columns * fontWidth) * fontHeight * (bpp / 8));
    end = ((columns * fontWidth) * (rows * fontHeight - fontHeight) * (bpp / 8)) / sizeof(uint64_t);
    dest = (uint64_t *) shadowBuf;

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = src[i];
    }

    // Clear the last line
    dest = (uint64_t *) (shadowBuf + (columns * fontWidth) * (rows * fontHeight - fontHeight) * (bpp / 8));
    end = (columns * fontWidth) * fontHeight * (bpp / 8) / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = 0;
    }

    // Write buffer to screen memory
    src = (uint64_t *) shadowBuf;
    dest = (uint64_t *) LinearFrameBuffer::hardwareBuffer;
    end = LinearFrameBuffer::xres * LinearFrameBuffer::yres * (bpp / 8) / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = src[i];
    }
}

void LfbText::clear () {
    LinearFrameBuffer::clear();
    setpos(0,0);
}
