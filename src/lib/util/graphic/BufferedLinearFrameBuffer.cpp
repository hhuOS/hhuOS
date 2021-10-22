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

#include <lib/util/memory/Address.h>
#include "BufferedLinearFrameBuffer.h"

namespace Util::Graphic {

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(uint32_t physicalAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t bitsPerPixel, uint16_t pitch) :
        LinearFrameBuffer(physicalAddress, resolutionX, resolutionY, bitsPerPixel, pitch),
        softwareBuffer(new uint8_t[pitch * resolutionY], pitch * resolutionY) {
    Memory::Address<uint32_t>(softwareBuffer).setRange(0, pitch * resolutionY);
}

BufferedLinearFrameBuffer::~BufferedLinearFrameBuffer() {
    delete[] reinterpret_cast<uint8_t*>(softwareBuffer.get());
}

Memory::Address<uint32_t> BufferedLinearFrameBuffer::getBuffer() const {
    return softwareBuffer;
}

void BufferedLinearFrameBuffer::flush() const {
    LinearFrameBuffer::getBuffer().copyRange(softwareBuffer, getPitch() * getResolutionY());
}

}