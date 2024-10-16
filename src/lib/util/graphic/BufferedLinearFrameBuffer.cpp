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

namespace Util::Graphic {

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, bool enableAcceleration) : BufferedLinearFrameBuffer(lfb, lfb.getPitch(), enableAcceleration) {}

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t pitch, bool enableAcceleration) :
        LinearFrameBuffer(new uint8_t[pitch * lfb.getResolutionY()], lfb.getResolutionX(), lfb.getResolutionY(), lfb.getColorDepth(), pitch),
        pitch(pitch), targetPitch(lfb.getPitch()),
        targetBuffer(enableAcceleration ? *Address<uint32_t>::createAcceleratedAddress(lfb.getBuffer().get(), useMmx) : *new Address<uint32_t>(lfb.getBuffer())) {
    clear();
}

BufferedLinearFrameBuffer::~BufferedLinearFrameBuffer() {
    delete &targetBuffer;
}

void BufferedLinearFrameBuffer::flush() const {
    if (pitch == targetPitch) {
        targetBuffer.copyRange(getBuffer(), getPitch() * getResolutionY());
    } else {
        auto source = getBuffer();
        auto target = targetBuffer;

        for (uint32_t i = 0; i < getResolutionY(); i++) {
            target.copyRange(source, pitch);
            target.add(pitch).setRange(0, targetPitch - pitch);

            source = source.add(pitch);
            target = target.add(targetPitch);
        }
    }

    if (useMmx) {
        Math::endMmx();
    }
}

}