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

#include <device/cpu/Cpu.h>
#include <util/memory/Address.h>
#include "LinearFrameBuffer.h"

namespace Util::Graphic {

LinearFrameBuffer::LinearFrameBuffer(void *address, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        buffer(static_cast<uint8_t*>(address)), resolutionX(resolutionX), resolutionY(resolutionY), bitsPerPixel(colorDepth), pitch(pitch) {}

uint16_t LinearFrameBuffer::getResolutionX() const {
    return resolutionX;
}

uint16_t LinearFrameBuffer::getResolutionY() const {
    return resolutionY;
}

uint8_t LinearFrameBuffer::getColorDepth() const {
    return bitsPerPixel;
}

uint16_t LinearFrameBuffer::getPitch() const {
    return pitch;
}

uint8_t *LinearFrameBuffer::getBuffer() const {
    return buffer;
}

Color LinearFrameBuffer::readPixel(uint16_t x, uint16_t y) const {
    if (x > resolutionX - 1 || y > resolutionY - 1) {
        Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_BOUNDS, "LinearFrameBuffer: Trying to read a pixel out of bounds!");
    }

    auto bpp = static_cast<uint8_t>(bitsPerPixel == 15 ? 16 : bitsPerPixel);
    uint8_t *ptr = getBuffer() + (x * (bpp / 8)) + y * pitch;

    return Color(*(reinterpret_cast<uint32_t*>(ptr)), bitsPerPixel);
}

}