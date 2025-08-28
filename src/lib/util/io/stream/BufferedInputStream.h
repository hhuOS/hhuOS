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

#ifndef HHUOS_LIB_UTIL_BUFFEREDINPUTSTREAM_H
#define HHUOS_LIB_UTIL_BUFFEREDINPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "io/stream/FilterInputStream.h"

namespace Util::Io {

/// A buffered input stream adds buffering to another input stream.
/// It maintains an internal buffer to reduce the number of read operations on the underlying stream.
/// An obvious use case is to wrap a file input stream with a buffered input stream
/// to reduce the number of system calls and improve performance.
///
/// ## Example
/// ```c++
/// Util::Io::FileInputStream fileStream("/path/to/file.txt");
/// Util::Io::BufferedInputStream bufferedStream(fileStream);
///
/// // Due to the implementation of OutputStream::readLine(), calling it on a non-buffered stream
/// // would result in a call to read() for each character in the line.
/// // Each of these read() calls would in turn result in a system call, leading to significant overhead.
/// // By using a BufferedInputStream, the number of system calls is reduced significantly,
/// // as the internal buffer is filled with multiple bytes in a single read operation.
/// // Subsequent read() calls can then be served from the internal buffer until it is exhausted.
/// auto line = bufferedStream.readLine();
///
/// // If the position of the underlying stream is changed directly,
/// // the internal buffer of the buffered stream may become inconsistent.
/// // In such cases, the buffer should be cleared manually to ensure that subsequent read operations
/// // reflect the current position of the underlying stream.
/// fileStream.setPosition(0); // Reset file stream to the beginning
/// bufferedStream.clearBuffer(); // Clear the internal buffer to ensure consistency
/// ```
class BufferedInputStream final : public FilterInputStream {

public:
    /// Create a buffered input stream instancce that wraps the given underlying stream.
    /// The buffer size is specified in bytes and defaults to 512 bytes if not provided.
    /// The instance does not take ownership of the underlying stream. It is the caller's responsibility to ensure
    /// that the underlying stream remains valid for the lifetime of this buffered input stream.
    explicit BufferedInputStream(InputStream &stream, size_t bufferSize = DEFAULT_BUFFER_SIZE);

    /// Destroy the buffered input stream instance and free the internal buffer.
    ~BufferedInputStream() override;

    /// Read a single byte from the stream.
    /// This method first checks if there is data available in the internal buffer.
    /// If the buffer is exhausted, it attempts to refill it from the underlying stream.
    /// If refilling the buffer fails (e.g., due to end of stream or an error),
    /// -1 is returned to indicate that no more data is available.
    /// Refilling the buffer may block if the underlying stream has no data available currently.
    /// On success, the byte is returned as an integer in the range 0 to 255.
    int16_t read() override;

    /// Read up to length bytes from the stream into the target buffer, starting at the given offset.
    /// This method reads data from the internal buffer and refills it from the underlying stream as needed.
    /// If the internal buffer is exhausted and refilling fails (e.g., due to end of stream or an error),
    /// it aborts reading and returns the number of bytes read so far.
    /// If no bytes could be read at all, -1 is returned to indicate that no more data is available.
    /// Refilling the buffer may block if the underlying stream has no data available currently.
    /// It is the caller's responsibility to ensure that the target buffer has enough space for offset + length bytes.
    /// If the buffer is too small, data is written out of bounds, leading to undefined behavior.
    int32_t read(uint8_t *target, size_t offset, size_t length) override;

    /// Peek at the next byte in the stream without removing it from the stream.
    /// This method first checks if there is data available in the internal buffer.
    /// If the buffer is exhausted, it attempts to refill it from the underlying stream.
    /// If refilling the buffer fails (e.g., due to end of stream or an error),
    /// -1 is returned to indicate that no more data is available.
    /// Refilling the buffer may block if the underlying stream has no data available currently.
    /// On success, the byte is returned as an integer in the range 0 to 255.
	int16_t peek() override;

    /// Check if there is data available to read from the stream.
    /// This method first checks if there is data available in the internal buffer.
    /// If the buffer is exhausted, it checks if the underlying stream has data available.
    /// If this method returns true, a subsequent read call is guaranteed to succeed without blocking.
    bool isReadyToRead() override;

    /// Clear the internal buffer.
    /// This method can be used to discard any buffered data and ensure that subsequent read operations
    /// reflect the current position of the underlying stream.
    /// It is useful if the position of the underlying stream has been changed directly
    /// (e.g. by calling FileInputStream::setPosition()), as the internal buffer may become inconsistent in such cases.
    void clearBuffer();

private:

    bool refill();

    uint8_t *buffer;
    size_t size;
    size_t position = 0;
    size_t valid = 0;

    static constexpr size_t DEFAULT_BUFFER_SIZE = 512;
};

}

#endif
