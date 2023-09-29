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

#include "PixelDrawer.h"

#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Exception.h"

namespace Util::Graphic {

PixelDrawer::PixelDrawer(const LinearFrameBuffer &lfb): lfb(lfb), lfbBuffer(reinterpret_cast<uint8_t*>(lfb.getBuffer().get())), lfbPitch(lfb.getPitch()) {
    switch (lfb.getColorDepth()) {
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

void PixelDrawer::drawPixel(uint16_t x, uint16_t y, const Color &color) const {
    // Pixels outside the visible area won't be drawn
    if (x >= lfb.getResolutionX() || y >= lfb.getResolutionY()) {
        return;
    }

    // Invisible pixels won't be drawn
    if (color.getAlpha() == 0) {
        return;
    }

    // Blend if necessary and draw pixel
    if (color.getAlpha() < 255) {
        drawFunction(lfbBuffer, lfbPitch, x, y, lfb.readPixel(x, y).blend(color));
    } else {
        drawFunction(lfbBuffer, lfbPitch, x, y, color);
    }
}

void PixelDrawer::drawPixel15Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    const auto offset = x + y * (pitch / 2);
    reinterpret_cast<uint16_t*>(buffer)[offset] = color.getRGB15();
}

void PixelDrawer::drawPixel16Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    const auto offset = x + y * (pitch / 2);
    reinterpret_cast<uint16_t*>(buffer)[offset] = color.getRGB16();
}

void PixelDrawer::drawPixel24Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    uint32_t rgbColor = color.getRGB24();
    const auto offset = x * 3 + y * pitch;

    buffer[offset] = rgbColor & 0xff;
    buffer[offset + 1] = (rgbColor >> 8) & 0xff;
    buffer[offset + 2] = (rgbColor >> 16) & 0xff;
}

void PixelDrawer::drawPixel32Bit(uint8_t *const buffer, const uint16_t pitch, const uint16_t x, const uint16_t y, const Color &color) {
    const auto offset = x + y * (pitch / 4);
    reinterpret_cast<uint32_t*>(buffer)[offset] = color.getRGB32();
}

}