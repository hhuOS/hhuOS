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

#ifndef HHUOS_LIB_UTIL_IO_FILTERINPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_FILTERINPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "PipedInputStream.h"
#include "util/io/stream/InputStream.h"

namespace Util {
namespace Io {

/// A filter input stream is a wrapper around another input stream.
/// In its base implementation, it simply forwards all calls to the underlying stream without any modification.
/// Subclasses can override methods to modify the behavior,
/// e.g., by adding buffering, compression, or other transformations.
///
/// ## Example
/// ```c++
/// // A custom filter input stream that discards any non-printable characters
/// class PrintableCharacterInputStream final : public Util::Io::FilterInputStream {
///
/// public:
///
///     explicit PrintableCharacterInputStream(InputStream &stream) : FilterInputStream(stream) {}
///
///     int16_t peek() override {
///         auto byte = FilterInputStream::peek();
///
///         // Read bytes from the underlying stream until a printable character is found
///         while (!Util::CharacterTypes::isPrintable(byte) && byte != -1) {
///             // Consume the non-printable character
///             FilterInputStream::read();
///             byte = FilterInputStream::peek();
///         }
///
///         return byte;
///     }
///
///     int16_t read() override {
///         const auto peekedChar = peek();
///         if (peekedChar == -1) {
///             return -1; // End of stream reached
///         }
///
///         // Consume the peeked character and return it
///         FilterInputStream::read();
///         return peekedChar;
///     }
///
///     int32_t read(uint8_t *targetBuffer, const size_t offset, const size_t length) override {
///         size_t bytesRead = 0;
///
///         while (bytesRead < length) {
///             // Read next printable character
///             const auto byte = read();
///             if (byte == -1) {
///                 break; // End of stream reached
///             }
///
///             targetBuffer[offset + bytesRead] = static_cast<uint8_t>(byte);
///             bytesRead++;
///         }
///
///         return bytesRead == 0 ? -1 : static_cast<int32_t>(bytesRead);
///     }
///
///     bool isReadyToRead() override {
///         return FilterInputStream::isReadyToRead();
///     }
/// };
/// ```
class FilterInputStream : public InputStream {

public:
    /// Create a new filter input stream instance that wraps the given underlying stream.
    /// The instance does not take ownership of the underlying stream. It is the caller's responsibility to ensure
    /// that the underlying stream remains valid for the lifetime of this filter input stream.
    explicit FilterInputStream(InputStream &stream) : stream(stream), deleteStream(false) {}

    /// Create a new filter input stream instance that wraps the given underlying stream.
    /// The given stream must be heap allocated and the instance takes ownership of it.
    /// This means, the underlying stream is automatically deleted by the destructor of this instance.
    explicit FilterInputStream(InputStream *stream) : stream(*stream), deleteStream(true) {}

    /// Destroy the filter input stream instance and free the underlying stream if owned.
    ~FilterInputStream() override {
        if (deleteStream) {
            delete &stream;
        }
    }

    /// Read a single byte from the stream by forwarding the call to the underlying stream.
    /// Subclasses can override this method to modify the behavior.
    /// On success, the byte is returned as an integer in the range 0 to 255.
    /// If an error occurs or the end of the stream is reached, -1 is returned.
    /// If no byte is currently available, but the end of the stream has not been reached,
    /// implementations may block until data is available.
    int16_t read() override {
        return stream.read();
    }

    /// Read up to length bytes from the stream into the target buffer, starting at the given offset,
    /// by forwarding the call to the underlying stream. Subclasses can override this method to modify the behavior.
    /// On success, the number of bytes read is returned (which may be less than length if fewer bytes are available).
    /// If an error occurs or the end of the stream is reached, -1 is returned.
    /// It is the caller's responsibility to ensure that the target buffer has enough space for offset + length bytes.
    /// If the buffer is too small, data is written out of bounds, leading to undefined behavior.
    /// Implementations should read as many bytes as possible. If no bytes are available currently,
    /// but the end of the stream has not been reached, implementations may block until data is available.
    int32_t read(uint8_t *targetBuffer, const size_t offset, const size_t length) override {
        return stream.read(targetBuffer, offset, length);
    }

    /// Peek at the next byte in the stream without removing it from the stream
    /// by forwarding the call to the underlying stream. Subclasses can override this method to modify the behavior.
    /// On success, the byte is returned as an integer in the range 0 to 255.
    /// If an error occurs or the end of the stream is reached, -1 is returned.
    /// If no byte is currently available, but the end of the stream has not been reached,
    /// implementations may block until data is available.
    int16_t peek() override {
        return stream.peek();
    }

    /// Check if there is data available to read from the stream by forwarding the call to the underlying stream.
    /// Subclasses can override this method to modify the behavior.
    /// If this method returns true, a subsequent read call is guaranteed to succeed without blocking.
    bool isReadyToRead() override {
        return stream.isReadyToRead();
    }

private:

    InputStream &stream;
    bool deleteStream;
};

}
}

#endif
