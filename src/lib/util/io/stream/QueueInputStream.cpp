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

#include "QueueInputStream.h"

#include "lib/util/collection/Queue.h"

namespace Util::Io {

QueueInputStream::QueueInputStream(Queue<uint8_t> &queue, bool discardIfFull) : queue(queue), discardIfFull(discardIfFull) {}

int16_t QueueInputStream::read() {
    return queue.poll();
}

int32_t QueueInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        targetBuffer[offset + i] = queue.poll();
    }

    return length;
}

}