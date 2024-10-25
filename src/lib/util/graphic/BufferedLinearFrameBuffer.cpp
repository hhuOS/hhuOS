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

#include "lib/util/base/Address.h"
#include "lib/util/math/Math.h"
#include "BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/base/Exception.h"

namespace Util::Graphic {

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, bool enableAcceleration) :
    BufferedLinearFrameBuffer(lfb, lfb.getPitch(), enableAcceleration) {}

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t pitch, bool enableAcceleration) :
    BufferedLinearFrameBuffer(lfb, lfb.getResolutionX(), lfb.getResolutionY(), pitch, enableAcceleration) {}

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t resolutionX, uint16_t resolutionY, bool enableAcceleration) :
    BufferedLinearFrameBuffer(lfb, resolutionX, resolutionY, resolutionX * lfb.getBytesPerPixel(), enableAcceleration) {}

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t resolutionX, uint16_t resolutionY, uint16_t pitch, bool enableAcceleration) :
        LinearFrameBuffer(new uint8_t[pitch * resolutionY], resolutionX, resolutionY, lfb.getColorDepth(), pitch),
        scale(lfb.getResolutionX() / resolutionX > lfb.getResolutionY() / resolutionY ? lfb.getResolutionY() / resolutionY : lfb.getResolutionX() / resolutionX),
        offsetX((lfb.getResolutionX() - resolutionX * scale) / 2), offsetY((lfb.getResolutionY() - resolutionY * scale) / 2),
        target(lfb), targetBuffer(enableAcceleration ? *Address<uint32_t>::createAcceleratedAddress(lfb.getBuffer().get(), useMmx) : *new Address<uint32_t>(lfb.getBuffer())) {
    clear();
}

BufferedLinearFrameBuffer::~BufferedLinearFrameBuffer() {
    delete &targetBuffer;
}

void BufferedLinearFrameBuffer::flush() const {
    if (isCompatibleWith(target)) {
        targetBuffer.copyRange(getBuffer(), getPitch() * getResolutionY());
    } else if (getResolutionX() == target.getResolutionX() && getResolutionY() == target.getResolutionY()) {
        const auto pitch = getPitch();
        const auto targetPitch = target.getPitch();
        auto sourceAddress = getBuffer();
        auto targetAddress = targetBuffer;

        for (uint32_t i = 0; i < getResolutionY(); i++) {
            targetAddress.copyRange(sourceAddress, pitch);
            targetAddress.add(pitch).setRange(0, targetPitch - pitch);

            sourceAddress = sourceAddress.add(pitch);
            targetAddress = targetAddress.add(targetPitch);
        }
    } else {
        switch (target.getColorDepth()) {
            case 32:
                scalingFlush32();
                break;
            case 24:
                scalingFlush24();
                break;
            case 16:
            case 15:
                scalingFlush16();
                break;
            default:
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "BufferedLinearFrameBuffer: Unsupported color depth!");
        }
    }

    if (useMmx) {
        Math::endMmx();
    }
}

void BufferedLinearFrameBuffer::scalingFlush32() const {
    const uint16_t targetWidth = getResolutionX() * scale;
    const uint16_t targetHeight = getResolutionY() * scale;
    auto *sourceAddress = reinterpret_cast<uint32_t*>(getBuffer().get());
    auto *targetAddress = reinterpret_cast<uint32_t*>(targetBuffer.add(offsetX * 4 + offsetY * getPitch()).get());

    for (uint16_t y = 0; y < targetHeight; y++) {
        for (uint16_t x = 0; x < targetWidth; x++) {
            targetAddress[x] = sourceAddress[x / scale];
        }

        if (y % scale == 0 && y != 0) {
            sourceAddress += getPitch() / 4;
        }
        targetAddress += (target.getPitch()) / 4;
    }
}

void BufferedLinearFrameBuffer::scalingFlush24() const {
    const uint16_t targetWidth = getResolutionX() * scale;
    const uint16_t targetHeight = getResolutionY() * scale;
    auto *sourceAddress = reinterpret_cast<uint8_t*>(getBuffer().get());
    auto *targetAddress = reinterpret_cast<uint8_t*>(targetBuffer.add(offsetX * 3 + offsetY * getPitch()).get());

    for (uint16_t y = 0; y < targetHeight; y++) {
        for (uint16_t x = 0; x < targetWidth; x++) {
            const auto sourceIndex = (x / scale) * 3;
            const auto targetIndex = x * 3;

            targetAddress[targetIndex] = sourceAddress[sourceIndex];
            targetAddress[targetIndex + 1] = sourceAddress[sourceIndex + 1];
            targetAddress[targetIndex + 2] = sourceAddress[sourceIndex + 2];
        }

        if (y % scale == 0 && y != 0) {
            sourceAddress += getPitch();
        }
        targetAddress += (target.getPitch());
    }
}

void BufferedLinearFrameBuffer::scalingFlush16() const {
    const uint16_t targetWidth = getResolutionX() * scale;
    const uint16_t targetHeight = getResolutionY() * scale;
    auto *sourceAddress = reinterpret_cast<uint16_t*>(getBuffer().get());
    auto *targetAddress = reinterpret_cast<uint16_t*>(targetBuffer.add(offsetX * 2 + offsetY * getPitch()).get());

    for (uint16_t y = 0; y < targetHeight; y++) {
        for (uint16_t x = 0; x < targetWidth; x++) {
            targetAddress[x] = sourceAddress[x / scale];
        }

        if (y % scale == 0 && y != 0) {
            sourceAddress += getPitch() / 2;
        }
        targetAddress += (target.getPitch()) / 2;
    }
}

}