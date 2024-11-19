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
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/base/String.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Constants.h"
#include "lib/util/collection/Array.h"

namespace Util::Graphic {

LinearFrameBuffer::LinearFrameBuffer(uint32_t physicalAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        buffer(mapBuffer(reinterpret_cast<void*>(physicalAddress), resolutionY, pitch)),
        resolutionX(resolutionX), resolutionY(resolutionY), colorDepth(colorDepth), pitch(pitch) {}

LinearFrameBuffer::LinearFrameBuffer(void *virtualAddress, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        buffer(virtualAddress), resolutionX(resolutionX), resolutionY(resolutionY), colorDepth(colorDepth), pitch(pitch) {}

LinearFrameBuffer::LinearFrameBuffer(Util::Address<uint32_t> *address, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) :
        buffer(address), resolutionX(resolutionX), resolutionY(resolutionY), colorDepth(colorDepth), pitch(pitch) {}

LinearFrameBuffer::LinearFrameBuffer(Io::File &file) {
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

    uint32_t address = Util::String::parseInt(static_cast<const char*>(addressString));
    resolutionX = Util::String::parseInt(static_cast<const char*>(resolutionSplit[0]));
    resolutionY = Util::String::parseInt(static_cast<const char*>(resolutionSplit[1]));
    colorDepth = Util::String::parseInt(static_cast<const char*>(colorDepthSplit[1]));
    pitch = Util::String::parseInt(static_cast<const char*>(pitchString));
    buffer = mapBuffer(reinterpret_cast<void *>(address), resolutionY, pitch);
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
    auto address = buffer.add((x * (bpp / 8)) + y * pitch);

    return Color::fromRGB(*(reinterpret_cast<uint32_t*>(address.get())), colorDepth);
}

void LinearFrameBuffer::clear() const {
    buffer.setRange(0, getPitch() * getResolutionY());
}

Address<uint32_t> LinearFrameBuffer::mapBuffer(void *physicalAddress, uint16_t resolutionY, uint16_t pitch) {
    if (reinterpret_cast<uint32_t>(physicalAddress) % Util::PAGESIZE != 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "LinearFrameBuffer: Physical address is not page aligned!");
    }

    const auto size = resolutionY * pitch;
    const auto pageCount = size % Util::PAGESIZE == 0 ? (size / Util::PAGESIZE) : (size / Util::PAGESIZE) + 1;
    void *virtualAddress = mapIO(physicalAddress, pageCount);

    return Address<uint32_t>(virtualAddress);
}

uint8_t LinearFrameBuffer::getBytesPerPixel() const {
    return (colorDepth == 15 ? 16 : colorDepth) / 8;
}

bool LinearFrameBuffer::isCompatibleWith(const LinearFrameBuffer &other) const {
    return resolutionX == other.resolutionX && resolutionY == other.resolutionY && colorDepth == other.colorDepth && pitch == other.pitch;
}

}