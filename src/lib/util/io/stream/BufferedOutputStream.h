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

#ifndef HHUOS_LIB_UTIL_BUFFEREDOUTPUTSTREAM_H
#define HHUOS_LIB_UTIL_BUFFEREDOUTPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "io/stream/FilterOutputStream.h"

namespace Util::Io {

/// A buffered output stream adds buffering to another output stream.
/// It maintains an internal buffer to reduce the number of write operations on the underlying stream.
/// An obvious use case is to wrap a file output stream with a buffered output stream
/// to reduce the number of system calls and improve performance.
///
//// ## Example
/// ```c++
/// // Using the file output stream directly would result in a system call
/// // with significant overhead for each write() call.
/// // By using a buffered output stream, the number of system calls can be reduced significantly,
/// // as the internal buffer is filled before being flushed to the underlying stream.
/// // This can lead to a substantial performance improvement, especially when writing many small pieces of data.
/// Util::Io::FileOutputStream fileStream("/path/to/file.txt");
/// Util::Io::BufferedOutputStream bufferedStream(fileStream);
///
/// // Read up to 10 lines from standard input and write them to the file
/// auto line = Util::System::in.readLine();
/// for (int i = 1; i < 10 && !line.endOfFile; i++) {
///     bufferedStream.write(static_cast<const uint8_t*>(line.content), 0, line.content.length());
///     bufferedStream.write(static_cast<const uint8_t*>("\n"), 0, 1);
///     line = Util::System::in.readLine();
/// }
///
/// // Flush any remaining data in the buffer to the underlying stream
/// bufferedStream.flush();
/// ```
class BufferedOutputStream final : public FilterOutputStream {

public:
    /// Create a buffered output stream instance that wraps the given underlying stream.
    /// The buffer size is specified in bytes and defaults to 512 bytes if not provided.
    /// The instance does not take ownership of the underlying stream. It is the caller's responsibility to ensure
    /// that the underlying stream remains valid for the lifetime of this buffered input stream.
    explicit BufferedOutputStream(OutputStream &stream, size_t size = DEFAULT_BUFFER_SIZE);

    /// Destroy the buffered output stream instance and free the internal buffer.
    /// This will also flush any remaining data in the buffer to the underlying stream.
    ~BufferedOutputStream() override;

    /// Write a single byte to the stream.
    /// This method adds the byte to the internal buffer.
    /// If the internal buffer is full, it is flushed to the underlying stream first.
    /// On success, true is returned.
    /// If an error occurs (i.e. flushing the buffer fails), false is returned.
    bool write(uint8_t byte) override;

    /// Write up to length bytes from the sourceBuffer, starting at the given offset, to the stream.
    /// If the data fits into the remaining space in the internal buffer, it is copied into the buffer.
    /// If the data is larger than the remaining buffer space, the buffer is flushed first,
    /// and then the data is written directly to the underlying stream.
    /// The number of bytes actually written is returned,
    /// which may be less than length if the stream cannot accept more data.
    size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

    /// Flush any buffered data to the underlying stream and the number of bytes flushed.
    /// If flushing fails (e.g., due to an error in the underlying stream),
    /// the number of bytes successfully flushed before the error occurred is returned.
    size_t flush() override;

private:

    uint8_t *buffer;
    size_t size;
    size_t position = 0;

    static constexpr size_t DEFAULT_BUFFER_SIZE = 512;
};

}

#endif
