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

#ifndef HHUOS_LIB_UTIL_IO_QUEUEINPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_QUEUEINPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "util/collection/Queue.h"
#include "util/io/stream/InputStream.h"

namespace Util {
namespace Io {

/// An input stream that reads data from a queue.
/// This is useful for scenarios where data is produced asynchronously and needs to be consumed in a streaming manner.
/// The queue input stream reads bytes from the provided queue, blocking if necessary until data is available.
class QueueInputStream final : public InputStream {

public:
	/// Create a queue input stream instance that reads data from the given queue.
	/// The instance does not take ownership of the queue. It is the caller's responsibility to ensure
	/// that the queue remains valid for the lifetime of this filter input stream.
	explicit QueueInputStream(Queue<uint8_t> &queue) : queue(queue) {}

	/// Read a single byte from the underlying queue.
	/// If the queue is empty, this method will block until data is available.
	/// As the queue has no failing mechanism, this method will never return -1.
	int16_t read() override {
		return queue.poll();
	}

	/// Read length bytes from the underlying queue into the target buffer, starting at the given offset.
	/// If the queue has fewer than length bytes available, this method will block until enough data is available.
	/// As the queue has no failing mechanism, this method will always return the amount of bytes requested.
	/// It is the caller's responsibility to ensure that the target buffer has enough space for offset + length bytes.
	/// If the buffer is too small, data is written out of bounds, leading to undefined behavior.
	int32_t read(uint8_t *targetBuffer, const size_t offset, const size_t length) override {
		for (size_t i = 0; i < length; i++) {
			targetBuffer[offset + i] = queue.poll();
		}

		return static_cast<int32_t>(length);
	}

	/// Peek at the next byte in the queue without removing it.
	/// If the queue is empty, this method will block until data is available.
	/// As the queue has no failing mechanism, this method will never return -1.
	int16_t peek() override {
		return queue.peek();
	}

	/// Check if there is data available to read from the queue.
	/// This method returns true if the queue is not empty, false otherwise.
	/// If this method returns true, a subsequent read call is guaranteed to succeed without blocking.
	bool isReadyToRead() override {
		return !queue.isEmpty();
	}

private:

	Queue<uint8_t> &queue;
};

}
}

#endif
