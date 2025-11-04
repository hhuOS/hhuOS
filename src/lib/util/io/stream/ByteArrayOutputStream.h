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

#ifndef HHUOS_LIB_UTIL_IO_BYTEARRAYOUTPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_BYTEARRAYOUTPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "util/base/String.h"
#include "util/io/stream/OutputStream.h"

namespace Util::Io {

/// An output stream that writes data to a byte array in memory.
/// The byte array can be provided at construction time or allocated internally.
/// If the byte array is allocated internally, it will grow as needed when more data is written.
/// Otherwise, if a fixed-size buffer is provided, writes that exceed the buffer size will fail.
/// The stream maintains a current position within the byte array, which is advanced as data is written.
/// The current position in the byte array can be queried, which indicates how much data has been written.
class ByteArrayOutputStream final : public OutputStream {

public:
	/// Create a byte array output stream instance with an internally allocated buffer of the specified initial size.
	/// The buffer will grow as needed when more data is written.
	/// If no size is specified, a default initial size of 32 bytes will be used.
    explicit ByteArrayOutputStream(size_t size = DEFAULT_BUFFER_SIZE);

	/// Create a byte array output stream instance that writes to the given buffer.
	/// As no buffer size is specified, this stream might write out of bounds if too much data is written.
	/// It is the caller's responsibility to ensure that the buffer is large enough for the intended writes.
	/// The instance does not take ownership of the buffer. It is the caller's responsibility to ensure
	/// that the buffer remains valid for the lifetime of this output stream.
	explicit ByteArrayOutputStream(uint8_t *buffer);

	/// Create a byte array output stream instance that writes to the given buffer of specified size.
	/// If more data is written than the buffer can hold, the write operations will fail.
	/// The instance does not take ownership of the buffer. It is the caller's responsibility to ensure
	/// that the buffer remains valid for the lifetime of this output stream.
	ByteArrayOutputStream(uint8_t *buffer, size_t size);

	/// Destroy the byte array output stream and free the internal buffer if it was allocated.
	~ByteArrayOutputStream() override;

	/// Write a single byte to the buffer.
	/// If the buffer is full and was not allocated internally, the operation will fail and false is returned.
	/// Otherwise, the buffer will grow as needed to accommodate the new byte.
	bool write(uint8_t byte) override;

	/// Write up to length bytes from the sourceBuffer, starting at the given offset, to the buffer.
	/// The number of bytes actually written is returned,
	/// which may be less than length if the buffer cannot accept more data.
	/// If the buffer was not allocated internally and is full, no bytes will be written beyond its size.
	/// Otherwise, the buffer will grow as needed to accommodate the new data.
	size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

	/// Get the current position in the buffer, i.e., how many bytes have been written so far.
    [[nodiscard]] size_t getPosition() const;

	/// Check if the buffer is empty, i.e., no data has been written yet.
    [[nodiscard]] bool isEmpty() const;

	/// Get a pointer to the internal buffer.
	/// If the buffer was not allocated internally, this will return the buffer provided at construction time.
	/// Otherwise, the pointer might become invalid if the buffer has grown due to writes.
    [[nodiscard]] uint8_t* getBuffer() const;

	/// Write the contents of the buffer to the provided target buffer, up to the specified length.
	/// If length is less than the amount of data written, only length bytes will be copied.
	/// It is the caller's responsibility to ensure that the target buffer has enough space for length bytes.
	/// If the target buffer is too small, data will be written out of bounds, leading to undefined behavior.
    void getContent(uint8_t *target, size_t length) const;

	/// Get the contents of the buffer as a String.
	/// The returned String contains all bytes written so far.
	/// If the buffer contains null bytes or invalid characters, they will be included in the String as-is.
    [[nodiscard]] String getContent() const;

	/// Reset the stream to the beginning of the buffer.
	/// Any previously written data will be discarded and overwritten by subsequent writes.
	/// The buffer itself is not cleared or modified; only the current position is reset.
    void reset();

private:

    size_t ensureRemainingCapacity(size_t);

    uint8_t *buffer;
    size_t size = 0;
    size_t position = 0;
	
	bool allocatedBuffer = true;
	bool checkBounds = true;

    static constexpr uint32_t DEFAULT_BUFFER_SIZE = 32;
};

}

#endif
