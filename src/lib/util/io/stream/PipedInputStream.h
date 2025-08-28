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

#ifndef HHUOS_LIB_UTIL_IO_PIPEDINPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_PIPEDINPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "async/Spinlock.h"
#include "io/stream/InputStream.h"

namespace Util::Io {

class PipedOutputStream;

/// A piped input stream must be connected to a piped output stream to create a communication pipe.
/// Data written to the piped output stream can then be read from the piped input stream.
/// Bytes are read from the input stream in the order in which they were written to the output stream.
/// The input stream uses an internal buffer to store the data.
/// If the internal buffer is empty, read operations will block until data is available.
/// If the internal buffer is full, write operations on the connected output stream will block until
/// data has been read from the input stream.
/// In a single-threaded context, this can lead to a deadlock: When the buffer is full and a write operation
/// is performed on the output stream, while no read operation is performed on the input stream,
/// the write operation will block forever.
/// The internal buffer is synchronized using a spinlock, so multiple threads can perform read and write
/// operations concurrently. However, this class should not be used in interrupt handlers, as it may lead to
/// a deadlock if the interrupt occurs while the spinlock is held.
///
/// ## Example
/// ```c++
/// // Create two connected piped streams
/// auto inputStream = Util::Io::PipedInputStream();
/// auto outputStream = Util::Io::PipedOutputStream();
/// inputStream.connect(outputStream);
///
/// // Write data to the output stream
/// const auto *message = "Hello, World!";
/// outputStream.write(reinterpret_cast<const uint8_t*>(message), 0, 13);
///
/// // Read data from the input stream
/// // This will read the exact 13 bytes we just wrote to the output stream
/// auto buffer = new uint8_t[13];
/// inputStream.read(buffer, 0, 13);
///
/// // Print the read message (should print "Hello, World!")
/// Util::System::out << reinterpret_cast<const char*>(buffer)
///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
/// ```
class PipedInputStream final : public InputStream {

public:
	/// Create a piped input stream instance with the given buffer size.
	/// The stream is not yet connected to a piped output stream.
	/// Any read operation performed before connecting to an output stream will fire a panic.
	/// To connect to an output stream, use the `connect()` method.
    explicit PipedInputStream(size_t bufferSize = DEFAULT_BUFFER_SIZE);

	/// Create a piped input stream instance and connect it to the given piped output stream.
	/// The given buffer size is used for the internal buffer.
    explicit PipedInputStream(PipedOutputStream &outputStream, size_t bufferSize = DEFAULT_BUFFER_SIZE);

	/// Destroy the piped input stream and free the internal buffer.
    ~PipedInputStream() override;

	/// Connect this piped input stream to the given piped output stream.
	/// If this stream is already connected to an output stream, a panic is fired.
    void connect(PipedOutputStream &outputStream);

	/// Read a single byte from the stream and return it as an integer in the range 0 to 255.
	/// If no byte is currently available, the call will block until data is available.
	/// If the stream is not yet connected to an output stream, a panic is fired.
    int16_t read() override;

	/// Read `length` bytes from the stream into the given target buffer, starting at the given offset
	/// and return the number of bytes read. As there are no error cases, this should always return `length`.
	/// If fewer than `length` bytes are currently available, the call will block until enough data is available.
	/// If the stream is not yet connected to an output stream, a panic is fired.
	int32_t read(uint8_t *targetBuffer, size_t offset, size_t length) override;

	/// Peek at the next byte in the stream without removing it from the stream.
	/// If no byte is currently available, the call will block until data is available.
	/// If the stream is not yet connected to an output stream, a panic is fired.
	int16_t peek() override;

	/// Check if there is data available to read from the stream.
	/// This method returns immediately and does not block.
	/// If the stream is not yet connected to an output stream, a panic is fired.
    bool isReadyToRead() override;

	/// Return the number of bytes that can be read from the stream without blocking
	/// (i.e. the number of bytes currently stored in the internal buffer).
    size_t getReadableBytes();

private:

    friend class PipedOutputStream;

	bool write(uint8_t byte);

    size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length);

	size_t getWritableBytes();

    PipedOutputStream *source = nullptr;

    Async::Spinlock lock;

    uint8_t *buffer;
    size_t bufferSize;

    int32_t inPosition = -1;
    int32_t outPosition = 0;

    static constexpr size_t DEFAULT_BUFFER_SIZE = 1024;

};

}

#endif
