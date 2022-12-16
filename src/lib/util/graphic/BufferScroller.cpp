/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/memory/Address.h"
#include "lib/util/math/Math.h"
#include "BufferScroller.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

namespace Util::Graphic {

BufferScroller::BufferScroller(const LinearFrameBuffer &lfb, bool enableAcceleration) :
        lfb(lfb), targetBuffer(enableAcceleration ? *Memory::Address<uint32_t>::createAcceleratedAddress(lfb.getBuffer().get(), useMmx) : *new Memory::Address<uint32_t>(lfb.getBuffer())) {}

BufferScroller::~BufferScroller() {
    delete &targetBuffer;
}

void BufferScroller::scrollUp(uint16_t lineCount) const {
    // Move screen buffer upwards by the given amount of lines
    auto source = lfb.getBuffer().add(lfb.getPitch() * lineCount);
    targetBuffer.copyRange(source, lfb.getPitch() * (lfb.getResolutionY() - lineCount));
    if (useMmx) {
        Math::Math::endMmx();
    }

    // Clear lower part of the screen
    auto clear = lfb.getBuffer().add(lfb.getPitch() * (lfb.getResolutionY() - lineCount));
    clear.setRange(0, lfb.getPitch() * lineCount);
}

}