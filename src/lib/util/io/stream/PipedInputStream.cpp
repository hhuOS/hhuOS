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

#include "lib/util/base/Address.h"
#include "lib/util/base/Exception.h"
#include "PipedOutputStream.h"
#include "PipedInputStream.h"
#include "lib/util/async/Thread.h"

namespace Util::Io {

PipedInputStream::PipedInputStream() : buffer(new uint8_t[BUFFER_SIZE]) {}

PipedInputStream::PipedInputStream(PipedOutputStream &outputStream) : buffer(new uint8_t[BUFFER_SIZE]) {
    connect(outputStream);
}

PipedInputStream::~PipedInputStream() {
    delete[] buffer;
}

void PipedInputStream::connect(PipedOutputStream &outputStream) {
    if (source != nullptr) {
        Exception::throwException(Exception::ILLEGAL_STATE, "PipedOutputStream: Already connected!");
    }

    source = &outputStream;
    source->sink = this;
}

int16_t PipedInputStream::read() {
    uint8_t ret;
    uint32_t count = read(&ret, 0, 1);
    return count == 0 ? 0 : ret;
}

int32_t PipedInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    if (source == nullptr) {
        Exception::throwException(Exception::ILLEGAL_STATE, "PipedOutputStream: Source is null!");
    }

    if (length == 0) {
        return 0;
    }

    // Block while buffer is empty
    while (inPosition < 0) {
        Async::Thread::yield();
    }

    uint32_t remaining = length;
    uint32_t ret = 0;

    while (true) {
        // Calculate the amount of bytes we can copy at once
        uint32_t toCopy;
        if (outPosition < inPosition) {
            toCopy = remaining < static_cast<uint32_t>(inPosition - outPosition) ? remaining : (inPosition - outPosition);
        } else {
            toCopy = remaining < (BUFFER_SIZE - outPosition) ? remaining : (BUFFER_SIZE - outPosition);
        }

        // Copy bytes from internal buffer to targetBuffer buffer
        auto sourceAddress = Address<uint32_t>(buffer).add(outPosition);
        auto targetAddress = Address<uint32_t>(targetBuffer).add(offset);
        targetAddress.copyRange(sourceAddress, toCopy);

        offset += toCopy;
        remaining -= toCopy;
        ret += toCopy;

        if (outPosition == BUFFER_SIZE) {
            outPosition = 0;
        }

        // Wrap around, if we have reached the buffer's end
        if (static_cast<int32_t>(outPosition + toCopy) == inPosition) {
            inPosition = -1;
            outPosition = 0;
        } else {
            outPosition += toCopy;
        }

        // Check if we have copied the requested amount of bytes or if the internal buffer is empty
        if (remaining == 0 || inPosition == -1) {
            return ret;
        }
    }
}

void PipedInputStream::write(uint8_t c) {
    write(&c, 0, 1);
}

void PipedInputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    uint32_t sourcePosition = offset;
    uint32_t remaining = length;

    while (remaining > 0) {
        // Block while buffer is full
        while (inPosition == outPosition) {
            Async::Thread::yield();
        }

        if (inPosition < 0) { // Buffer is empty
            inPosition = 0;
        }

        // Calculate the amount of bytes we can copy at once
        uint32_t toCopy;
        if (inPosition < outPosition) {
            toCopy = remaining < static_cast<uint32_t>(outPosition - inPosition) ? remaining : (outPosition - inPosition);
        } else {
            toCopy = remaining < (BUFFER_SIZE - inPosition) ? remaining : (BUFFER_SIZE - inPosition);
        }

        // Copy bytes from sourceBuffer to internal buffer
        auto sourceAddress = Address<uint32_t>(sourceBuffer).add(sourcePosition);
        auto targetAddress = Address<uint32_t>(buffer).add(inPosition);
        targetAddress.copyRange(sourceAddress, toCopy);

        remaining -= toCopy;
        sourcePosition += toCopy;
        inPosition += toCopy;

        // Wrap around, if we have reached the buffer's end
        if (inPosition == BUFFER_SIZE) {
            inPosition = 0;
        }
    }
}

}