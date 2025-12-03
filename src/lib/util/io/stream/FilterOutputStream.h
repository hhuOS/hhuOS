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

#ifndef HHUOS_LIB_UTIL_IO_FILTEROUTPUTSTREAM_H
#define HHUOS_LIB_UTIL_IO_FILTEROUTPUTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "util/io/stream/OutputStream.h"

namespace Util {
namespace Io {

/// A filter output stream is a wrapper around another output stream.
/// In its base implementation, it simply forwards all calls to the underlying stream without any modification.
/// Subclasses can override methods to modify the behavior,
/// e.g., by adding buffering, compression, or other transformations.
///
/// ## Example
/// ```c++
/// // A custom filter output stream that only forwards printable characters to the underlying stream
/// class PrintableCharacterOutputStream final : public Util::Io::FilterOutputStream {
///
/// public:
///
///     explicit PrintableCharacterOutputStream(OutputStream &stream) : FilterOutputStream(stream) {}
///
///     bool write(uint8_t byte) override {
///         if (Util::CharacterTypes::isPrintable(byte)) {
///             // Write any printable character to the underlying stream
///             return FilterOutputStream::write(byte);
///         }
///
///         // Discard the byte if it is not printable
///         return true;
///     }
///
///     size_t write(const uint8_t *sourceBuffer, const size_t offset, const size_t length) override {
///         size_t bytesWritten = 0;
///
///         for (size_t i = 0; i < length; i++) {
///             if (Util::CharacterTypes::isPrintable(sourceBuffer[offset + i])) {
///                 if (FilterOutputStream::write(sourceBuffer[offset + i])) {
///                     bytesWritten++;
///                 } else {
///                     break; // Stop writing if an error occurs
///                 }
///             } else {
///                 bytesWritten++; // Count discarded non-printable characters as "written"
///             }
///         }
///
///         return bytesWritten;
///     }
/// };
/// ```
class FilterOutputStream : public OutputStream {

public:
    /// Create a new filter output stream instance that wraps the given underlying stream.
    /// The instance does not take ownership of the underlying stream. It is the caller's responsibility to ensure
    /// that the underlying stream remains valid for the lifetime of this filter output stream.
    explicit FilterOutputStream(OutputStream &stream) : stream(stream) {}

    /// Write a single byte to the stream by forwarding the call to the underlying stream.
    /// Subclasses can override this method to modify the behavior.
    /// On success, true is returned.
    /// If an error occurs, false is returned.
    bool write(const uint8_t byte) override {
        return stream.write(byte);
    }

    /// Write up to length bytes from the sourceBuffer, starting at the given offset, to the stream
    /// by forwarding the call to the underlying stream. Subclasses can override this method to modify the behavior.
    /// The number of bytes actually written is returned,
    /// which may be less than length if the stream cannot accept more data.
    size_t write(const uint8_t *sourceBuffer, const size_t offset, const size_t length) override {
        return stream.write(sourceBuffer, offset, length);
    }

    /// If the stream uses buffering, this method should flush any buffered data to the underlying destination.
    /// This implementation forwards the call to the underlying stream.
    /// Subclasses can override this method to modify the behavior.
    /// The number of bytes flushed is returned.
    size_t flush() override {
        return stream.flush();
    }

private:

    OutputStream &stream;
};

}
}

#endif
