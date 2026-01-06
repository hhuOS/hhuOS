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

#ifndef HHUOS_LIB_UTIL_IO_INPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_INPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "lib/util/base/String.h"

namespace Util {
namespace Io {

/// Base class for input streams. An input stream is a source for reading data.
/// The actual source can be a file, memory or any other data source, depending on the subclass.
/// Data is treated as a stream of raw bytes by the interface read methods.
class InputStream {

public:
    /// Represents a line read from the stream, including its content and whether the end of the file was reached.
    struct Line {
        /// The content of the line, excluding the newline character.
        String content;
        /// True if the line is empty and the end of the file was reached, false otherwise.
        bool endOfFile;
    };

    /// No state needs to be initialized in the base class, so the default constructor is sufficient.
    InputStream() = default;

    /// Streams are not copyable, as subclasses might manage resources that cannot be copied.
    InputStream(const InputStream &copy) = delete;

    /// Streams are not copyable, as subclasses might manage resources that cannot be copied.
    InputStream& operator=(const InputStream &copy) = delete;

    /// Destroy the input stream and free the skipping buffer, if allocated.
    virtual ~InputStream() {
        delete skipBuffer;
    }

    /// Read a single byte from the stream.
    /// On success, the byte is returned as an integer in the range 0 to 255.
    /// If an error occurs or the end of the stream is reached, -1 is returned.
    /// If no byte is currently available, but the end of the stream has not been reached,
    /// implementations may block until data is available.
    virtual int16_t read() = 0;

    /// Read up to length bytes from the stream into the target buffer, starting at the given offset.
    /// On success, the number of bytes read is returned (which may be less than length if fewer bytes are available).
    /// If an error occurs or the end of the stream is reached, -1 is returned.
    /// It is the caller's responsibility to ensure that the target buffer has enough space for offset + length bytes.
    /// If the buffer is too small, data is written out of bounds, leading to undefined behavior.
    /// Implementations should read as many bytes as possible. If no bytes are available currently,
    /// but the end of the stream has not been reached, implementations may block until data is available.
    virtual int32_t read(uint8_t *targetBuffer, size_t offset, size_t length) = 0;

    /// Peek at the next byte in the stream without removing it from the stream.
    /// On success, the byte is returned as an integer in the range 0 to 255.
    /// If an error occurs or the end of the stream is reached, -1 is returned.
    /// If no byte is currently available, but the end of the stream has not been reached,
    /// implementations may block until data is available.
    virtual int16_t peek() = 0;

    /// Check if there is data available to read from the stream.
    /// If this method returns true, a subsequent read call is guaranteed to succeed without blocking.
    virtual bool isReadyToRead() = 0;

    /// Read length bytes from the stream and return them as a String.
    String readString(size_t length);

    /// Read bytes from the stream until a newline character ('\n') is encountered or the end of the stream is reached.
    /// The read bytes are returned as a String, excluding the newline character.
    Line readLine();

    /// Skip over and discard amount bytes from the stream.
    /// This is done by repeatedly reading into an internal buffer until amount bytes have been skipped
    /// or the end of the stream is reached. The number of bytes actually skipped is returned.
    size_t skip(size_t amount);

private:

    uint8_t *skipBuffer = nullptr;

    static constexpr size_t SKIP_BUFFER_SIZE = 1024;
};

}
}

#endif
