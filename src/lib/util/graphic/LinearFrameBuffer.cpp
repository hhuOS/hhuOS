/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <lib/util/Exception.h>
#include <lib/interface.h>
#include "LinearFrameBuffer.h"

namespace Util::Graphic {

LinearFrameBuffer::LinearFrameBuffer(uint32_t physicalAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        buffer(mapIO(physicalAddress, pitch * resolutionY), pitch * resolutionY),
        resolutionX(resolutionX), resolutionY(resolutionY), colorDepth(colorDepth), pitch(pitch) {}

LinearFrameBuffer::~LinearFrameBuffer() {
    delete[] reinterpret_cast<uint8_t*>(buffer.get());
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

Memory::Address<uint32_t> LinearFrameBuffer::getBuffer() const {
    return buffer;
}

Color LinearFrameBuffer::readPixel(uint16_t x, uint16_t y) const {
    if (x > resolutionX - 1 || y > resolutionY - 1) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "LinearFrameBuffer: Trying to read a pixel out of bounds!");
    }

    auto bpp = static_cast<uint8_t>(colorDepth == 15 ? 16 : colorDepth);
    auto address = buffer.add((x * (bpp / 8)) + y * pitch);

    return Color::fromRGB(*(reinterpret_cast<uint32_t*>(address.get())), colorDepth);
}

void LinearFrameBuffer::clear() {
    buffer.setRange(0, getPitch() * getResolutionY());
}

}