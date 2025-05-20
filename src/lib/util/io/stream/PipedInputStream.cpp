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

#include "lib/util/base/Address.h"
#include "lib/util/base/Panic.h"
#include "PipedOutputStream.h"
#include "PipedInputStream.h"
#include "lib/util/async/Thread.h"

namespace Util::Io {

PipedInputStream::PipedInputStream(int32_t bufferSize) : buffer(new uint8_t[bufferSize]), bufferSize(bufferSize) {}

PipedInputStream::PipedInputStream(PipedOutputStream &outputStream, int32_t bufferSize) : buffer(new uint8_t[bufferSize]), bufferSize(bufferSize) {
    connect(outputStream);
}

PipedInputStream::~PipedInputStream() {
    delete[] buffer;
}

void PipedInputStream::connect(PipedOutputStream &outputStream) {
    if (source != nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "PipedOutputStream: Already connected!");
    }

    source = &outputStream;
    source->sink = this;
}

int16_t PipedInputStream::read() {
    uint8_t ret;
	
	if (peekedCharacter != -1) {
		int16_t ret = peekedCharacter;
		peekedCharacter = -1;
		return ret;
	}
	
    uint32_t count = read(&ret, 0, 1);
    return count == 0 ? 0 : ret;
}

int16_t PipedInputStream::peek() {
	if (peekedCharacter == -1) peekedCharacter = read();
	return peekedCharacter;
}

int32_t PipedInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    if (source == nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "PipedOutputStream: Source is null!");
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

    uint32_t remaining = length;
    uint32_t ret = 0;

    while (true) {
        // Calculate the amount of bytes we can copy at once
        uint32_t toCopy;
        if (outPosition < inPosition) {
            toCopy = remaining < static_cast<uint32_t>(inPosition - outPosition) ? remaining : (inPosition - outPosition);
        } else {
            toCopy = remaining < (static_cast<uint32_t>(bufferSize) - outPosition) ? remaining : (bufferSize - outPosition);
        }

        // Copy bytes from internal buffer to targetBuffer buffer
        auto sourceAddress = Address(buffer).add(outPosition);
        auto targetAddress = Address(targetBuffer).add(offset);
        targetAddress.copyRange(sourceAddress, toCopy);

        offset += toCopy;
        remaining -= toCopy;
        outPosition += toCopy;
        ret += toCopy;

        if (outPosition == bufferSize) {
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
        uint32_t toCopy;
        if (inPosition < outPosition) {
            toCopy = remaining < static_cast<uint32_t>(outPosition - inPosition) ? remaining : (outPosition - inPosition);
        } else {
            toCopy = remaining < (static_cast<uint32_t>(bufferSize) - inPosition) ? remaining : (bufferSize - inPosition);
        }

        // Copy bytes from sourceBuffer to internal buffer
        auto sourceAddress = Address(sourceBuffer).add(sourcePosition);
        auto targetAddress = Address(buffer).add(inPosition);
        targetAddress.copyRange(sourceAddress, toCopy);

        remaining -= toCopy;
        sourcePosition += toCopy;
        inPosition += toCopy;

        // Wrap around, if we have reached the buffer's end
        if (inPosition == bufferSize) {
            inPosition = 0;
        }
    }

    lock.release();
}

bool PipedInputStream::isReadyToRead() {
    return available() > 0;
}

uint32_t PipedInputStream::available() {
    uint32_t ret;
    lock.acquire();

    if (inPosition < 0) {
        ret = 0;
    } else if (outPosition < inPosition) {
        ret = inPosition - outPosition;
    } else {
        ret = (bufferSize - outPosition) + inPosition;
    }

    lock.release();
    return ret;
}

}