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

#include "PipedInputStream.h"

#include "util/async/Thread.h"
#include "util/base/Address.h"
#include "util/base/Panic.h"
#include "util/io/stream/PipedOutputStream.h"

namespace Util {
namespace Io {

void PipedInputStream::connect(PipedOutputStream &outputStream) {
    if (source != nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "PipedOutputStream: Already connected!");
    }

    source = &outputStream;
    source->sink = this;
}

int16_t PipedInputStream::read() {
    uint8_t ret;
	
    const auto count = read(&ret, 0, 1);
    if (count == 0) {
        return 0;
    }
    if (count < 0) {
        return -1;
    }

    return ret;
}

int32_t PipedInputStream::read(uint8_t *targetBuffer, size_t offset, size_t length) {
    if (source == nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "PipedOutputStream: Not connected to a source!");
    }

    if (length == 0) {
        return 0;
    }

    // Block while buffer is empty
    lock.acquire();
    while (inPosition < 0) {
        lock.release();
        Async::Thread::yield();
        lock.acquire();
    }

    size_t targetOffset = offset;
    size_t remaining = length;
    size_t readBytes = 0;

    while (true) {
        // Calculate the amount of bytes we can copy at once
        size_t toCopy;
        if (outPosition < inPosition) {
            toCopy = remaining < static_cast<size_t>(inPosition - outPosition) ? remaining : inPosition - outPosition;
        } else {
            toCopy = remaining < bufferSize - outPosition ? remaining : bufferSize - outPosition;
        }

        // Copy bytes from internal buffer to targetBuffer buffer
        auto sourceAddress = Address(buffer).add(outPosition);
        auto targetAddress = Address(targetBuffer).add(targetOffset);
        targetAddress.copyRange(sourceAddress, toCopy);

        targetOffset += toCopy;
        remaining -= toCopy;
        outPosition += static_cast<int32_t>(toCopy);
        readBytes += toCopy;

        if (static_cast<size_t>(outPosition) == bufferSize) {
            outPosition = 0;
        }

        // Wrap around, if we have reached the buffer's end
        if (outPosition == inPosition) {
            inPosition = -1;
            outPosition = 0;
        }

        // Check if we have copied the requested amount of bytes or if the internal buffer is empty
        if (remaining == 0 || inPosition == -1) {
            lock.release();
            return static_cast<int32_t>(readBytes);
        }
    }
}

int16_t PipedInputStream::peek() {
    if (source == nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "PipedOutputStream: Not connected to a source!");
    }

    // Block while buffer is empty
    lock.acquire();
    while (inPosition < 0) {
        lock.release();
        Async::Thread::yield();
        lock.acquire();
    }

    return outPosition < inPosition ? buffer[outPosition] : buffer[(outPosition + 1) % bufferSize];
}

bool PipedInputStream::write(const uint8_t byte) {
    return write(&byte, 0, 1) == 1;
}

size_t PipedInputStream::write(const uint8_t *sourceBuffer, const size_t offset, const size_t length) {
    size_t sourcePosition = offset;
    size_t remaining = length;
    lock.acquire();

    while (remaining > 0) {
        // Block while buffer is full
        while (inPosition == outPosition) {
            lock.release();
            Async::Thread::yield();
            lock.acquire();
        }

        if (inPosition < 0) { // Buffer is empty
            inPosition = 0;
        }

        // Calculate the amount of bytes we can copy at once
        size_t toCopy;
        if (inPosition < outPosition) {
            toCopy = remaining < static_cast<size_t>(outPosition - inPosition) ? remaining : outPosition - inPosition;
        } else {
            toCopy = remaining < bufferSize - inPosition ? remaining : bufferSize - inPosition;
        }

        // Copy bytes from sourceBuffer to internal buffer
        auto sourceAddress = Address(sourceBuffer).add(sourcePosition);
        auto targetAddress = Address(buffer).add(inPosition);
        targetAddress.copyRange(sourceAddress, toCopy);

        remaining -= toCopy;
        sourcePosition += toCopy;
        inPosition += static_cast<int32_t>(toCopy);

        // Wrap around, if we have reached the buffer's end
        if (static_cast<size_t>(inPosition) == bufferSize) {
            inPosition = 0;
        }
    }

    return lock.releaseAndReturn(length);
}

}
}
