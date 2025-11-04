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

#ifndef HHUOS_LIB_UTIL_IO_BYTEARRAYINPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_BYTEARRAYINPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "util/io/stream/InputStream.h"

namespace Util::Io {

/// An input stream that reads data from a byte array in memory.
/// The byte array is provided at construction time and is not modified by the stream.
/// The stream maintains a current position within the byte array, which is advanced as data is read.
/// When the end of the byte array is reached, further read attempts will return -1.
///
/// ### Example
/// ```c++
///	// Construct a byte array input stream from a string literal
/// const char *data = "abcdefg";
/// Util::Io::ByteArrayInputStream byteArrayStream(reinterpret_cast<const uint8_t*>(data), 7);
///
/// const auto length = byteArrayStream.getLength(); // length is 7
/// auto position = byteArrayStream.getPosition(); // position is 0
/// auto remaining = byteArrayStream.getRemaining(); // remaining is 7
///
/// const auto a = static_cast<char>(byteArrayStream.read()); // read 'a'
/// const auto b = static_cast<char>(byteArrayStream.read()); // read 'b'
/// const auto c = static_cast<char>(byteArrayStream.read()); // read 'c'
/// position = byteArrayStream.getPosition(); // position is 3
/// remaining = byteArrayStream.getRemaining(); // remaining is 4
///
/// char defg[4];
/// byteArrayStream.read(reinterpret_cast<uint8_t*>(defg), 0, 4); // read 'd', 'e', 'f', 'g'
/// position = byteArrayStream.getPosition(); // position is 7
/// remaining = byteArrayStream.getRemaining(); // remaining is 0
/// ```
class ByteArrayInputStream final : public InputStream {

public:
	/// Create a byte array input stream instance that reads from the given buffer of specified size.
	/// The instance does not take ownership of the buffer. It is the caller's responsibility to ensure
    /// that the buffer remains valid for the lifetime of the stream.
	ByteArrayInputStream(const uint8_t *buffer, size_t size);

	/// Create a byte array input stream instance that reads from the given buffer.
	/// Since no buffer size is specified, this instance will not perform bounds checking
	/// and continues to read bytes beyond the provided buffer. This may lead to undefined behavior.
	/// It is the caller's responsibility to ensure that the buffer is large enough for the intended reads.
    /// The instance does not take ownership of the buffer. It is the caller's responsibility to ensure
    /// that the buffer remains valid for the lifetime of the stream.
	explicit ByteArrayInputStream(const uint8_t *buffer);

	/// Read a single byte from the stream.
	/// This will return the next byte in the buffer and advance the position by one.
	/// If the end of the buffer is reached, -1 is returned.
    /// If null-termination checking is enabled (by calling `stopAtNullTerminator(true)`),
    /// reading will stop when a null terminator (`'\0'`) is encountered, returning 0.
    /// Subsequent reads will continue to return 0 until `stopAtNullTerminator(false)` is called.
    /// This is useful for reading null-terminated strings from the buffer.
	int16_t read() override;

	/// Read up to length bytes from the stream into the target buffer, starting at the given offset.
    /// The number of bytes actually read is returned.
	/// It is the caller's responsibility to ensure that the target buffer has enough space for offset + length bytes.
	/// If the buffer is too small, data is written out of bounds, leading to undefined behavior.
    /// If the end of the buffer is reached before reading length bytes, fewer bytes may be returned.
	/// If the end of the buffer is reached before reading any bytes, -1 is returned.
	int32_t read(uint8_t *targetBuffer, size_t offset, size_t length) override;

	/// Peek at the next byte in the buffer without advancing the position.
	/// If the end of the buffer is reached, -1 is returned.
	int16_t peek() override;

	/// Check if there is data available to read from the buffer.
	/// If no buffer size was specified at construction time, this method always returns true.
    /// Otherwise, it checks if the current position is less than the buffer size.
	bool isReadyToRead() override;

	/// Return the total length of the buffer in bytes.
    /// If no buffer size was specified at construction time, this method always returns 0.
    [[nodiscard]] size_t getLength() const;

	/// Return the current position within the buffer.
    [[nodiscard]] size_t getPosition() const;

	/// Return the number of bytes remaining to be read from the buffer.
	/// If no buffer size was specified at construction time, this method always returns 0.
    [[nodiscard]] size_t getRemaining() const;

	/// Check if the buffer has been completely read.
	/// If no buffer size was specified at construction time, this method always returns false.
    [[nodiscard]] bool isEmpty() const;

	/// Return a pointer to the underlying buffer.
    /// The returned pointer is the same as the one provided at construction time.
    [[nodiscard]] const uint8_t* getBuffer() const;

	/// Enable or disable stopping at null terminators (`'\0'`).
    /// When enabled, reading will stop when a null terminator is encountered, returning 0.
    /// Subsequent reads will continue to return 0 until this feature is disabled.
    /// By default, this feature is disabled.
	void stopAtNullTerminator(bool stop);

private:

    const uint8_t *buffer;
    const size_t size = 0;

    size_t position = 0;
	
	bool checkBounds = true;
	bool nullTerminated = false;
};

}

#endif
