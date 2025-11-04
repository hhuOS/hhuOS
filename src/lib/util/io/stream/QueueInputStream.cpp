/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "QueueInputStream.h"

#include "util/collection/Queue.h"

namespace Util::Io {

QueueInputStream::QueueInputStream(Queue<uint8_t> &queue) : queue(queue) {}

int16_t QueueInputStream::read() {
    return queue.poll();
}

int32_t QueueInputStream::read(uint8_t *targetBuffer, const size_t offset, const size_t length) {
    for (size_t i = 0; i < length; i++) {
        targetBuffer[offset + i] = queue.poll();
    }

    return static_cast<int32_t>(length);
}

int16_t QueueInputStream::peek() {
    return queue.peek();
}

bool QueueInputStream::isReadyToRead() {
    return !queue.isEmpty();
}

}