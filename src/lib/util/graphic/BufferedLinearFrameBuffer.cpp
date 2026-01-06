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

#include "BufferedLinearFrameBuffer.h"

#include "util/base/Address.h"
#include "util/base/Panic.h"
#include "util/graphic/LinearFrameBuffer.h"

namespace Util {
namespace Graphic {

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, const uint16_t resolutionX,
    const uint16_t resolutionY, const uint16_t pitch) :
    LinearFrameBuffer(new uint8_t[pitch * resolutionY], resolutionX, resolutionY, lfb.getColorDepth(), pitch), lfb(lfb)
{
    if (getResolutionX() > lfb.getResolutionX() || getResolutionY() > lfb.getResolutionY()) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT,
            "BufferedLinearFrameBuffer: Buffered resolution must not be larger than target resolution!");
    }

    if (lfb.getResolutionX() / resolutionX > lfb.getResolutionY() / resolutionY) {
        scale = lfb.getResolutionY() / resolutionY;
    } else {
        scale = lfb.getResolutionX() / resolutionX;
    }

    offsetX  = (lfb.getResolutionX() - resolutionX * scale) / 2;
    offsetY  = (lfb.getResolutionY() - resolutionY * scale) / 2;

    clear();
}

void BufferedLinearFrameBuffer::flush() const {
    const auto &targetBuffer = lfb.getBuffer();

    if (isCompatibleWith(lfb)) {
        targetBuffer.copyRange(getBuffer(), getPitch() * getResolutionY());
    } else if (getResolutionX() == lfb.getResolutionX() && getResolutionY() == lfb.getResolutionY()) {
        const auto pitch = getPitch();
        const auto targetPitch = lfb.getPitch();
        auto sourceAddress = getBuffer();
        auto targetAddress = targetBuffer;

        for (uint32_t i = 0; i < getResolutionY(); i++) {
            targetAddress.copyRange(sourceAddress, pitch);
            targetAddress.add(pitch).setRange(0, targetPitch - pitch);

            sourceAddress = sourceAddress.add(pitch);
            targetAddress = targetAddress.add(targetPitch);
        }
    } else {
        const uint16_t targetWidth = getResolutionX() * scale;
        const uint16_t targetHeight = getResolutionY() * scale;
        const auto colorDepth = getColorDepth();

        switch (colorDepth) {
            case 32:
            {
                const auto *sourceAddress = reinterpret_cast<uint32_t*>(getBuffer().get());
                auto *targetAddress = reinterpret_cast<uint32_t*>(lfb.getBuffer().add(offsetX * 4 + offsetY * lfb.getPitch()).get());

                for (uint16_t y = 0; y < targetHeight; y++) {
                    for (uint16_t x = 0; x < targetWidth; x++) {
                        targetAddress[x] = sourceAddress[x / scale];
                    }

                    sourceAddress += (y % scale == 0 && y != 0) * (getPitch() / 4);
                    targetAddress += lfb.getPitch() / 4;
                }

                return;
            }

            case 24:
            {
                const auto *sourceAddress = reinterpret_cast<uint8_t*>(getBuffer().get());
                auto *targetAddress = reinterpret_cast<uint8_t*>(lfb.getBuffer().add(offsetX * 3 + offsetY * lfb.getPitch()).get());

                for (uint16_t y = 0; y < targetHeight; y++) {
                    for (uint16_t x = 0; x < targetWidth; x++) {
                        const auto sourceIndex = x / scale * 3;
                        const auto targetIndex = x * 3;

                        targetAddress[targetIndex] = sourceAddress[sourceIndex];
                        targetAddress[targetIndex + 1] = sourceAddress[sourceIndex + 1];
                        targetAddress[targetIndex + 2] = sourceAddress[sourceIndex + 2];
                    }

                    sourceAddress += (y % scale == 0 && y != 0) * getPitch();
                    targetAddress += (lfb.getPitch());
                }

                return;
            }

            case 16:
            {
                const auto *sourceAddress = reinterpret_cast<uint16_t*>(getBuffer().get());
                auto *targetAddress = reinterpret_cast<uint16_t*>(lfb.getBuffer().add(offsetX * 2 + offsetY * lfb.getPitch()).get());

                for (uint16_t y = 0; y < targetHeight; y++) {
                    for (uint16_t x = 0; x < targetWidth; x++) {
                        targetAddress[x] = sourceAddress[x / scale];
                    }

                    sourceAddress += (y % scale == 0 && y != 0) * (getPitch() / 2);
                    targetAddress += lfb.getPitch() / 2;
                }

                return;
            }

            default:
                Util::Panic::fire(Panic::UNSUPPORTED_OPERATION,
                    "BufferedLinearFrameBuffer: Unsupported color depth for scaling flush!");
        }
    }
}

}
}