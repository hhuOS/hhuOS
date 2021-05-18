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
#include "BufferScroller.h"

namespace Util::Graphic {

BufferScroller::BufferScroller(LinearFrameBuffer &lfb) : lfb(lfb) {

}

void BufferScroller::scrollUp(uint16_t lineCount) {
    // Move screen buffer upwards by the given amount of lines
    auto source = lfb.getBuffer().add(lfb.getPitch() * lineCount);
    lfb.getBuffer().copyRange(source, lfb.getPitch() * (lfb.getResolutionY() - lineCount));

    // Clear lower part of the screen
    auto clear = lfb.getBuffer().add(lfb.getPitch() * (lfb.getResolutionY() - lineCount));
    clear.setRange(0, lfb.getPitch() * lineCount);
}

void BufferScroller::scrollDown(uint16_t lineCount) {
    // Move the screen buffer downwards line by line, starting at the bottom
    for (int32_t i = 1; i < lfb.getResolutionY() - lineCount; i++) {
        auto source = lfb.getBuffer().add(lfb.getPitch() * (lfb.getResolutionY() - lineCount - i));
        auto destination = lfb.getBuffer().add(lfb.getPitch() * (lfb.getResolutionY() - i - 1));
        destination.copyRange(source, lfb.getPitch());
    }

    // Clear upper part of the screen
    lfb.getBuffer().setRange(0, lfb.getPitch() * lineCount);
}

}